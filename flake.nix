{
  description = "An user-friendly Graphical Interface";

  inputs = {
    gepetto.url = "github:gepetto/nix";
    flakoboros.follows = "gepetto/flakoboros";
    gazebros2nix.follows = "gepetto/gazebros2nix";
    flake-parts.follows = "gepetto/flake-parts";
    nixpkgs.follows = "gepetto/nixpkgs";
    systems.follows = "gepetto/systems";
    treefmt-nix.follows = "gepetto/treefmt-nix";
  };

  outputs =
    inputs:
    inputs.flake-parts.lib.mkFlake { inherit inputs; } (
      { lib, ... }:
      {
        systems = import inputs.systems;
        imports = [
          inputs.gepetto.flakeModule
          {
            flakoboros = {
              pyOverrideAttrs.gepetto-viewer = _: _: { };
              overrideAttrs.gepetto-viewer = _: {
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
            };
          }
        ];
      }
    );
}
