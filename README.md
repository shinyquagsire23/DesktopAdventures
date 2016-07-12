## DesktopAdventures

### About

DesktopAdventures is a recreation of the Desktop Adventures engine used in *Indianna Jones and his Desktop Adventures* and *Yoda Stories* games, licensed under LGPLv2. It currently supports *nix and 3DS, with support for Windows and OS X planned, but currently not implemented. The end goal of this project is to provide a free and open source implementation of the Desktop Adventures engine so that these games can be playable on modern platforms and preserved for future generations.

### Platforms

Platform-specific code is placed within separate directories so as to keep the main code easily portable to any platform. Porting to another platform expects an implementation of OpenGL of some sort (SDL on PC-like platforms, ctrulib+Caelina for 3DS), and an output console for printf or similar. Input is handled platform-specific with hooks to the main code.

#### Compiling

For *nix/PC platforms, just compile using cmake:
```
mkdir build && cd build
cmake ..
make
```

For 3DS, cd to *src/3ds/* and run **make**.

### Work Needed

The majority of the work needed in this project is reverse engineering and the implementation of the research obtained from it. At the time of writing, the following sections of the .DAT are being properly parsed for *Yoda Stories* only:

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
  - IACT (see SCRIPTS.md)
- PUZ2 (puzzle-specific data)
  - IPUZ (currently has reading implemented, needs more research)
- ANAM (seems to have a lot of script command names? Looks to be debugging info for scripts, actually has a lot of fragments of leaked/overflowed command names seen in Yoda Stories. Indy only.)
- PNAM (Indy only)
- ZNAM (Indy only)

Most data on these sections can be implied and documented without the need of disassembly, however there are some aspects of the engine which will require this. IDA Pro is extremely useful for documenting certain aspects of various sections.
