{ pkgs ? import <nixpkgs> {} }:

pkgs.callPackage (

{ mkShell
, SDL2
, freetype
, libdrm
, libinput
, pkg-config
, gdb
}:

mkShell {
  buildInputs = [
    freetype

    SDL2

    libdrm
    libinput
  ];
  nativeBuildInputs = [
    pkg-config
    gdb
  ];
}

) { }
