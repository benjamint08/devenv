{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  packages = with pkgs; [
    cmake
    ninja
    gcc
    pkg-config
    curl
    nlohmann_json
  ];
}
