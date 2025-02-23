{
  description = "Flake for stc";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = inputs @ {flake-parts, ...}:
    flake-parts.lib.mkFlake {inherit inputs;} {
      systems = ["x86_64-linux" "aarch64-linux" "aarch64-darwin" "x86_64-darwin"];

      perSystem = {pkgs, ...}: let
        name = "stc";
        version = "0.0.1";
      in {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [gcc];
        };

        packages.default = pkgs.stdenv.mkDerivation {
          pname = name;
          inherit version;
          src = ./.;

          installPhase = ''
            mkdir -p $out/include
            cp stc.h $out/include
          '';
        };
      };
    };
}
