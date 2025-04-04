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
              stdenv ? pkgs.stdenv,
              prog-name ? "vmem-cache-sim",
              platforms ? allSystems,
            }:
            stdenv.mkDerivation {
              name = "${prog-name}";
              src = self;
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
        {
          default = builder { };
          clang = builder { stdenv = pkgs.clangStdenv; };
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
