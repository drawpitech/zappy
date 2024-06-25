{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = {self, ...} @ inputs:
    inputs.utils.lib.eachDefaultSystem (
      system: let
        pkgs = import inputs.nixpkgs {inherit system;};
        python = pkgs.python312;
      in {
        formatter = pkgs.alejandra;

        devShells.default = pkgs.mkShell {
          inputsFrom = builtins.attrValues self.packages.${system};
          packages = with pkgs; [clang-tools_18 strace aflplusplus];
          env.MAKEFLAGS = "-j12";
          env.LD_LIBRARY_PATH = "${pkgs.libGL}/lib";
        };

        packages = rec {
          zappy_server = pkgs.stdenv.mkDerivation {
            name = "zappy_server";
            src = ../../server;
            nativeBuildInputs = with pkgs; [cmake];
            enableParallelBuilding = true;
          };
          zappy_gui = pkgs.stdenv.mkDerivation {
            name = "zappy_gui";
            src = ../../GUI;
            nativeBuildInputs = with pkgs; [cmake makeWrapper pkg-config];
            buildInputs = with pkgs; [
              wayland
              wayland-protocols
              libxkbcommon

              assimp
              glfw
              libGL
              xorg.libX11
              xorg.libXcursor
              xorg.libXi
              xorg.libXinerama
              xorg.libXrandr
              zlib
            ];
            propagatedBuildInputs = with pkgs; [ libGL ];

            postInstall = ''
              wrapProgram $out/bin/zappy_gui \
                --prefix LD_LIBRARY_PATH : "${pkgs.libGL}/lib"
            '';
            enableParallelBuilding = true;
          };
          zappy_ai = python.pkgs.buildPythonPackage {
            pname = "zappy_ai";
            version = "0.1.0";
            src = ../../AI;
            pyproject = true;
            build-system = with python.pkgs; [setuptools];
            dependencies = with python.pkgs; [typer];
          };

          zappy_server_docker = pkgs.dockerTools.buildImage {
            name = "zappy_server";
            tag = "latest";
            created = "now";
            copyToRoot = pkgs.buildEnv {
              name = "image-root";
              paths = [zappy_server];
            };
            config.Cmd = ["zappy_server" "-p" "52001" "-x" "10" "-y" "10" "-n" "team1" "-c" "12" "-f" "100"];
          };

          reference_gui = pkgs.buildFHSUserEnv {
            name = "reference_gui-fhs-shell";
            targetPkgs = p: [p.sfml];
            runScript = "env --chdir=${../ref} -S ./zappy_gui";
          };
        };
      }
    );
}
