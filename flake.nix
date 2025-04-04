{
  description = "Virtual Memory Cache Simulator";
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };
  outputs =
    inputs@{ self, ... }:
    let
      allSystems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];

      forAllSystems =
        f: inputs.nixpkgs.lib.genAttrs allSystems (system: f (import inputs.nixpkgs { inherit system; }));
    in
    {
      packages = forAllSystems (
        pkgs:
        let
          prog-name = "vmem-cache-sim";
          platforms = allSystems;
          builder =
            {
              stdenv ? pkgs.stdenv,
              inner ? { },
            }:
            stdenv.mkDerivation (
              {
                name = "${prog-name}";
                src = self;
                checkPhase = ''
                  ls -alh
                '';
                installPhase =
                  let
                    outpath = # bash
                      "$out/bin/${prog-name}";
                  in
                  ''
                    echo "$buildPhase"
                    mkdir -p "$out/bin"
                    mv vmem-cache-sim "${outpath}"
                    test -x "${outpath}"
                  '';
                meta = {
                  mainProgram = "${prog-name}";
                  platforms = platforms;
                };

              }
              // inner
            );
        in
        {
          default = builder { };
          clang = builder { stdenv = pkgs.clangStdenv; };
          cmake = builder {
            inner = {
              buildInputs = [ pkgs.cmake ];
            };
          };
        }
      );
      checks = forAllSystems (pkgs: {
        clang-tidy = pkgs.runCommand "clang-tidy" { } ''
          ${pkgs.clang-tools}/bin/clang-tidy ${./src}/**/*.c} ${./src}/**/*.h
          touch $out
        '';
        toplevel-makefile-not-generated-by-cmake = pkgs.runCommand "makefile-checker" { } ''
          read -r first_line < ${./Makefile}
          first_line_lower="''${first_line,,}"
          if [[ "$first_line_lower" = *"cmake generated file"* ]]; then
            echo "The toplevel Makefile has been overwritten by CMake! Revert the change!"
            echo "Got first line as $first_line_lower"
            exit 1
          fi
          touch $out
        '';
      });
      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          packages = with pkgs; [
            gnumake
            gcc
            clang-tools
            clang
          ];
        };
      });
    };
}
