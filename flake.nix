{
  description = "An user-friendly Graphical Interface";

  inputs = {
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    inputs:
    inputs.flake-parts.lib.mkFlake { inherit inputs; } {
      systems = inputs.nixpkgs.lib.systems.flakeExposed;
      perSystem =
        { pkgs, self', ... }:
        {
          devShells.default = pkgs.mkShell { inputsFrom = [ self'.packages.default ]; };
          packages = {
            default = self'.packages.gepetto-viewer;
            gepetto-viewer = pkgs.python3Packages.gepetto-viewer.overrideAttrs (_: {
              # TODO: remove this after next release
              patches = [];
              src = pkgs.lib.fileset.toSource {
                root = ./.;
                fileset = pkgs.lib.fileset.unions [
                  ./cmake-module
                  ./CMakeLists.txt
                  ./doc
                  ./fonts
                  ./include
                  ./package.xml
                  ./plugins
                  ./pyplugins
                  ./res
                  ./src
                  ./tests
                ];
              };
            });
          };
        };
    };
}
