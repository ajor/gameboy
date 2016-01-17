# Gameboy Emulator

This repository contains a Gameboy emulator core with a choice of two frontends:
- [Native binary](#native-binary), only tested on Linux
- [Google Chrome Native Client (NaCl)](#nacl) application

# Completeness / accuracy

Save files are compatible with other popular Gameboy emulators.

Audio is not implemented :(

Memory bank controllers (MBCs) 1 and 3 are the only ones currently supported, although the vast majority of Gameboy games use one of these two (or no MBC at all). The Real Time Clock (RTC) used in MBC3 is not implemented, so games which use it to keep track of time while the Gameboy is powered off will not function correctly in some aspects.

Scanlines are rendered individually, with each scanline being rendered in one go at the end of the H-Blank period. This means games which modify the data to be drawn mid-scanline do not render correctly - e.g. Prehistorik Man title sequence.

## Blargg's tests
TODO results

# Native binary

## Building

### Requirements
- A C++14 compiler
- CMake 2.8.12+
- GLFW3
- GLEW

### Compilation
Compile using CMake (defaults to release configuration):
```
mkdir build
cd build
cmake ../
make
```

## Usage

    ./gb rom

### Controls

The Gameboy's direction keys are mapped to the arrow keys.
Start button = enter
Select button = backspace
A button = z
B button = x

Keyboard controls are currently not customisable at runtime.

# NaCl

The Native Client version of this emulator is not yet complete, and is currently lacking any form of button input.

A pre-built version is available for use at http://ajor.co.uk/gameboy/nacl/

## Building

### Requirements
- [Chrome Native Client SDK](https://developer.chrome.com/native-client)
- CMake 2.8.12+

### Compilation
Compile using CMake, replacing `[NaCl toolchain bin dir]` with the path to the PNaCl toolchain directory for your platform, e.g.: `~/nacl_sdk/pepper_46/toolchain/linux_pnacl/bin`
```
mkdir build
cd build
cmake ../ -DNACL_TOOLCHAIN_BIN=[NaCl toolchain bin dir] -DCMAKE_TOOLCHAIN_FILE=../nacl/NaCl.cmake
make
```

## Usage
Place the generated gb.pexe binary along with nacl/index.html and nacl/gameboy.nmf from this repository on a web server and open in Google Chrome.

It can be run from your own computer using Python's HTTP server module:

    python2 -m SimpleHTTPServer
or:

    python3 -m http.server
