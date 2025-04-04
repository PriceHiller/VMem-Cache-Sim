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
          builder =
            {
              CC,
              buildInputs,
              prog-name ? "vmem-cache-sim",
              platforms ? allSystems,
            }:
            pkgs.stdenv.mkDerivation {
              name = "${prog-name}";
              src = self;
              buildInputs = buildInputs;
              buildPhase = ''
                echo "Building with ${CC}"
                make CC="${CC}" output-bin="${prog-name}" build-release
              '';
              installPhase =
                let
                  outpath = # bash
                    "$out/bin/${prog-name}";
                in
                ''
                  mkdir -p "$out/bin"
                  mv vmem-cache-sim "${outpath}"
                  test -x "${outpath}"
                '';
              meta = {
                mainProgram = "${prog-name}";
                platforms = platforms;
              };

            };
        in
        rec {
          gcc = builder {
            CC = "gcc";
            buildInputs = [ pkgs.gcc ];
          };
          clang = builder {
            CC = "clang";
            buildInputs = [ pkgs.clang ];
          };
          default = clang;
        }
      );
      checks = forAllSystems (pkgs: {
        clang-tidy = pkgs.runCommand "clang-tidy" { } ''
          ${pkgs.clang-tools}/bin/clang-tidy ${./src}/**/*.c} ${./src}/**/*.h
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
