# Dungeon Generator (dungeongen)

C++17 port and recreation of Dungeon Generator v1.0, originally developed by Linux Cyberjoueurs in 2002.

## Disclaimer and Attribution

This project is a clean-room reverse-engineered implementation of the original `dungeongen.exe` utility, reconstructed through Ghidra disassembly. 

- **Original Copyright**: The original software, design concepts, tile specifications, and trademarks belong to **Linux Cyberjoueurs**.
- **Purpose**: Created solely for educational purposes, software preservation, and cross-platform compilation on modern systems.
- **Attribution**: No ownership is claimed over original file formats (`.nw`, `.gmap`), tile encoding specifications, or associated engine assets.

## Overview

The tool parses configuration options from a text file, generates a dungeon layout (either from custom ASCII files, procedural maze algorithms, or room stats), applies auto-tiling rules to place walls/floors, and outputs level files and map links compatible with 2D tile map servers.

### Key Components

- **Level File Generation (`GLEVNW01`)**: Produces 64x64 tile level files using 6-bit Base64 tile encoding.
- **Map File Generation (`GRMAP001`)**: Generates `.gmap` files linking adjacent levels together into a contiguous grid.
- **Auto-Tiling Engine**: Evaluates 8-neighbor cell adjacencies (N, S, E, W, NW, NE, SW, SE) to match 15 wall-shape templates (`wall`, `walln`, `walle`, `walls`, `wallw`, `wallne`, `wallnw`, `wallse`, `wallsw`, `wallne2`, `wallnw2`, `wallse2`, `wallsw2`, `walld1`, `walld2`).
- **Layout Algorithms**: Includes implementation of all original procedural algorithms:
  - `onion`: Nested ring structures.
  - `maze`: Cell-carving maze generation.
  - `spiral`: Circular trigonometric paths.
  - `rogue`: 3x3 room and corridor grids.
  - `snake`: Alternating corridor paths.
  - `squarespiral`: Rectangular spiral layouts.
- **NPC Placement**: Rule-based NPC spawning based on cell type, position, and probability.

## Building

### Requirements

- CMake 3.14 or later
- C++17 compatible compiler (MSVC, GCC, or Clang)

### Build Instructions

```bash
cmake -B build -S .
cmake --build build --config Release
```

The compiled binary will be written to `build/Release/dungeongen.exe` (or `build/dungeongen` on Unix platforms).

## Usage

```bash
# Run with default option file (dungeonoptions.txt in current directory)
./dungeongen

# Run with a custom options file
./dungeongen path/to/options.txt
```

### Options File Format (`dungeonoptions.txt`)

```text
GDUNG001
LEVELSTART testhouse_
GMAP testhouse

WALLTILES tiles_wallshorror.png
GROUNDTILES tiles_groundhorror.png

# Layout selection
LAYOUTFILE Dungeon.txt
# Alternatively, set procedural options:
# LAYOUTSTYLE maze
# LAYOUTWIDTH 20
# LAYOUTHEIGHT 20

# Wall tile definition templates
TILES wall=0xFF9,0xFF9,...
TILES walln=...

# NPC definitions
NPC script=light2.txt,blocking=false,underplayer=true,chance=40%,place=path,x=0,y=0
```

## License

Distributed under the MIT License for community preservation and educational use.
