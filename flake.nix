{
  description = "An user-friendly Graphical Interface";

  inputs.gepetto.url = "github:gepetto/nix";

  outputs =
    inputs:
    inputs.gepetto.lib.mkFlakoboros inputs (
      { lib, ... }:
      {
        extraDevPyPackages = [ "gepetto-viewer" ];
        overrideAttrs.gepetto-viewer = {
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
      }
    );
}
