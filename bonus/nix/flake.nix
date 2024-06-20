{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = {self, ...} @ inputs:
    inputs.utils.lib.eachDefaultSystem (
      system: let
        pkgs = import inputs.nixpkgs {inherit system;};
      in {
        formatter = pkgs.alejandra;

        devShells = {
          default = pkgs.mkShell {
            inputsFrom = builtins.attrValues self.packages.${system} ++ (with pkgs; [glfw assimp]);
            packages = with pkgs; [clang-tools_18 strace];
            env.MAKEFLAGS = "-j12";
          };
          fhs = (pkgs.buildFHSUserEnv {
            name = "fhs-shell";
            targetPkgs = p: with p; [sfml] ;
          }).env;
        };

        packages = {
          zappy_server = pkgs.stdenv.mkDerivation {
            name = "zappy_server";
            src = ../../server;
            nativeBuildInputs = with pkgs; [cmake libuuid];
            enableParallelBuilding = true;
          };
        };
      }
    );
}
