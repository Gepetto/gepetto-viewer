{
  description = "An user-friendly Graphical Interface";

  inputs = {
    gepetto.url = "github:gepetto/nix";
    flake-parts.follows = "gepetto/flake-parts";
    nixpkgs.follows = "gepetto/nixpkgs";
    nix-ros-overlay.follows = "gepetto/nix-ros-overlay";
    systems.follows = "gepetto/systems";
    treefmt-nix.follows = "gepetto/treefmt-nix";
  };

  outputs =
    inputs:
    inputs.flake-parts.lib.mkFlake { inherit inputs; } {
      systems = import inputs.systems;
      imports = [ inputs.gepetto.flakeModule ];
      perSystem =
        {
          lib,
          pkgs,
          self',
          ...
        }:
        {
          packages =
            let
              override = {
                src = lib.fileset.toSource {
                  root = ./.;
                  fileset = lib.fileset.unions [
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
              };
            in
            {
              default = self'.packages.py-gepetto-viewer;
              gepetto-viewer = pkgs.gepetto-viewer.overrideAttrs override;
              py-gepetto-viewer = pkgs.python3Packages.gepetto-viewer.overrideAttrs override;
            };
        };
    };
}
