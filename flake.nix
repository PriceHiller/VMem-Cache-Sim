{
  description = "Virtual Memory Cache Simulator";
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    treefmt-nix.url = "github:numtide/treefmt-nix";
  };
  outputs =
    inputs@{ self, systems, ... }:
    let
      eachSystem =
        f:
        inputs.nixpkgs.lib.genAttrs (import systems) (
          system: f (import inputs.nixpkgs { inherit system; })
        );
      treefmtEval = eachSystem (pkgs: inputs.treefmt-nix.lib.evalModule pkgs ./treefmt.nix);
      lib = inputs.nixpkgs.lib;
    in
    {
      packages =
        let
          prog-name = "vmem-cache-sim";
          outpath = # bash
            "$out/bin/${prog-name}";
        in
        eachSystem (
          pkgs:
          let
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
                  installPhase = ''
                    mkdir -p "$out/bin"
                    mv vmem-cache-sim "${outpath}"
                    test -x "${outpath}"
                  '';
                  meta = {
                    mainProgram = "${prog-name}";
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
                installPhase = # bash
                  ''
                    mkdir -p "$out/bin"
                    mv "bin/${prog-name}" "${outpath}"
                    test -x "${outpath}"
                  '';

              };
            };
          }
        );
      formatter = eachSystem (pkgs: treefmtEval.${pkgs.system}.config.build.wrapper);
      checks = eachSystem (
        pkgs:
        let
          clang-tools = pkgs.llvmPackages_latest.clang-tools;
          src-files = builtins.filter (f: (lib.strings.hasSuffix ".c" f || lib.strings.hasSuffix ".c" f)) (
            lib.filesystem.listFilesRecursive "${self}/src"
          );
          src-files-string = lib.strings.concatStringsSep " " (
            builtins.map (file: ''"${builtins.toString file}"'') src-files
          );
        in
        {
          formatting = treefmtEval.${pkgs.system}.config.build.check self;
          clang-tidy = pkgs.runCommand "clang-tidy" { } ''
            echo "SELF: ${self}"
            ${clang-tools}/bin/clang-tidy ${src-files-string}
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
        }
      );
      devShells = eachSystem (pkgs: {
        default = pkgs.mkShell {
          packages = with pkgs; [
            gnumake
            gcc
            llvmPackages_latest.clang-tools
            llvmPackages_latest.clang
          ];
        };
      });
    };
}
