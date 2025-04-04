# treefmt.nix
{ pkgs, ... }:
{
  projectRootFile = "flake.nix";
  programs.clang-format = {
    enable = true;
    package = pkgs.llvmPackages_latest.clang-tools;
  };
  programs.nixfmt.enable = true;
}
