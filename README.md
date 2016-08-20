## DesktopAdventures

### About

DesktopAdventures is a recreation of the Desktop Adventures engine used in *Indianna Jones and his Desktop Adventures* and *Yoda Stories* games, licensed under LGPLv2. It currently supports OS X, *nix, 3DS, and Wii U, with support for Windows currently not tested. The end goal of this project is to provide a free and open source implementation of the Desktop Adventures engine so that these games can be playable on modern platforms and preserved for future generations.

### Platforms

Platform-specific code is placed within separate directories so as to keep the main code easily portable to any platform. Porting to another platform expects an implementation of the C standard library such as glibc or newlib, as well as a framebuffer to draw in (hardware acceleration is available through the RENDER_GL backend). Input is handled platform-specific with hooks to the main code.

#### Compiling

For *nix/PC platforms, just compile using cmake:
```
mkdir build && cd build
cmake ..
make
```

For 3DS, cd to *src/3ds/* and run **make**. Compiling for 3DS requires an installation of DevKitARM and ctrulib.

For Wii U, cd to *src/wiiu/* and run **make**. Compiling for Wii U requires an installation of DevKitPPC and wut, in addition to newlib being built with *-fno-jump-tables* (see [here](https://github.com/devkitPro/buildscripts/issues/19)).

### Work Needed

#### OS Porting

Currently DesktopAdventures has only been developed and verified to work for Linux and 3DS (however it may work on OS X). Contributors who can validate and check on support for other platforms such as Windows or any other platforms which may be desirable to port to are welcome.

#### DAT Research

The majority of the work needed in this project is reverse engineering and the implementation of the research obtained from it. At the time of writing, the following sections of the .DAT are being properly parsed:

- VERS
- STUP
- SNDS
- TILE
- ZONE
  - IZON
  - IZAX
  - IZX2
  - IZX3
  - IZX4
- CHAR
  - ICHA
- CHWP
- CAUX
- TNAM

The following currently need implementation and/or research:

- ZONE
  - IACT (see SCRIPTS.md, iact.c)
- PUZ2 (puzzle-specific data)
  - IPUZ (currently has reading implemented, needs more research)
- ANAM (seems to have a lot of script command names? Looks to be debugging info for scripts, actually has a lot of fragments of leaked/overflowed command names seen in Yoda Stories. Indy only.)
- PNAM (Indy only)
- ZNAM (Indy only)

Most data on these sections can be implied and documented without the need of disassembly, however there are some aspects of the engine which will require this. IDA Pro is extremely useful for documenting certain aspects of various sections.
