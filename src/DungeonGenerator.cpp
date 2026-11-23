#include "DungeonGenerator.h"
#include "Utils.h"
#include <iostream>
#include <ctime>
#include <cstdlib>

static const char* WALL_SHAPE_BY_MASK[256] = {
    "path", "walln", "wallne", "walln", "walle", "wallne2", "walle", "wallne2",
    "wallse", "wallne2", "walle", "wallne2", "walle", "wallne2", "walle", "wallne2",
    "walls", "wall", "wallse2", "wall", "wallse2", "wall", "wallse2", "wall",
    "walls", "wall", "wallse2", "wall", "wallse2", "wall", "wallse2", "wall",
    "wallsw", "wallnw2", "walld2", "wallnw2", "wallse2", "wall", "wallse2", "wall",
    "walls", "wall", "wallse2", "wall", "wallse2", "wall", "wallse2", "wall",
    "walls", "wall", "wallse2", "wall", "wallse2", "wall", "wallse2", "wall",
    "walls", "wall", "wallse2", "wall", "wallse2", "wall", "wallse2", "wall",
    "wallw", "wallnw2", "wallnw2", "wallnw2", "wall", "wall", "wall", "wall",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall",
    "wallw", "wallnw2", "wallnw2", "wallnw2", "wall", "wall", "wall", "wall",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall",
    "wallnw", "walln", "walln", "walln", "wallne2", "wallne2", "wallne2", "wallne2",
    "walld1", "wallne2", "wallne2", "wallne2", "wallne2", "wallne2", "wallne2", "wallne2",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall",
    "wallw", "wallnw2", "wallnw2", "wallnw2", "wall", "wall", "wall", "wall",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall",
    "wallw", "wallnw2", "wallnw2", "wallnw2", "wall", "wall", "wall", "wall",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall",
    "wallw", "wallnw2", "wallnw2", "wallnw2", "wall", "wall", "wall", "wall",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall",
    "wallsw2", "wall", "wall", "wall", "wall", "wall", "wall", "wall"
};

void DungeonGenerator::run(const std::string& optionsFilename) {
    std::cout << "*****************************************************************\n";
    std::cout << "* Dungeon Generator v1.0 (C) 2002 Linux Cyberjoueurs, All right Reserved\n";
    std::cout << "* Can only be used to generate levels for the game Graalonline\n";
    std::cout << "*****************************************************************\n\n";

    std::cout << "Starting dungeon creation\n";
    std::cout << "Loading options\n" << std::flush;
    if (!loadOptions(optionsFilename)) return;

    std::cout << "Loading dungeon definition\n" << std::flush;
    if (!loadDungeonDefinition()) return;

    std::cout << "Creating levels\n" << std::flush;
    createLevels();

    std::cout << "Generating the dungeon\n" << std::flush;
    generateDungeon();

    std::cout << "Writing levels\n" << std::flush;
    writeLevels();

    std::cout << "Writing the gmap\n" << std::flush;
    writeGMap();

    std::cout << "Dungeon creation finished, please hit return to end.\n" << std::flush;
    std::cin.get();
}

bool DungeonGenerator::loadOptions(const std::string& optionsFilename) {
    return options.loadFromFile(optionsFilename);
}

bool DungeonGenerator::loadDungeonDefinition() {
    if (options.randomSeed == 0) {
        unsigned int seed = static_cast<unsigned int>(std::time(nullptr));
        std::cout << "Generating new seed value:" << seed << "\n";
        Utils::seedRandom(seed);
    } else {
        std::cout << "Seed loaded from file value:" << options.randomSeed << "\n";
        Utils::seedRandom(options.randomSeed);
    }

    if (!layout.generate(options)) {
        return false;
    }

    // Calculate level grid dimensions
    levelsX = (layout.width + 7) / 8;
    levelsY = (layout.height * 11 + 63) / 64;
    if (levelsX <= 0) levelsX = 1;
    if (levelsY <= 0) levelsY = 1;

    return true;
}

void DungeonGenerator::createLevels() {
    levels.assign(levelsX, std::vector<Level>(levelsY));

    for (int y = 0; y < levelsY; ++y) {
        for (int x = 0; x < levelsX; ++x) {
            Level& lvl = levels[x][y];
            lvl.xIndex = x;
            lvl.yIndex = y;
            lvl.name = Level::formatLevelName(options.levelStart, x, y, levelsX, levelsY);

            // Add links to neighboring levels
            if (x > 0) { // Link West
                Link l;
                l.targetLevel = Level::formatLevelName(options.levelStart, x - 1, y, levelsX, levelsY);
                l.x = 0; l.y = 0; l.w = 1; l.h = 64;
                l.newX = "61"; l.newY = "playery";
                lvl.links.push_back(l);
            }
            if (x + 1 < levelsX) { // Link East
                Link l;
                l.targetLevel = Level::formatLevelName(options.levelStart, x + 1, y, levelsX, levelsY);
                l.x = 63; l.y = 0; l.w = 1; l.h = 64;
                l.newX = "0"; l.newY = "playery";
                lvl.links.push_back(l);
            }
            if (y > 0) { // Link North
                Link l;
                l.targetLevel = Level::formatLevelName(options.levelStart, x, y - 1, levelsX, levelsY);
                l.x = 0; l.y = 0; l.w = 64; l.h = 1;
                l.newX = "playerx"; l.newY = "61";
                lvl.links.push_back(l);
            }
            if (y + 1 < levelsY) { // Link South
                Link l;
                l.targetLevel = Level::formatLevelName(options.levelStart, x, y + 1, levelsX, levelsY);
                l.x = 0; l.y = 63; l.w = 64; l.h = 1;
                l.newX = "playerx"; l.newY = "0";
                lvl.links.push_back(l);
            }
        }
    }
}

std::string DungeonGenerator::determineWallShape(int gx, int gy) const {
    auto isWall = [this](int x, int y) -> bool {
        if (x < 0 || x >= layout.width || y < 0 || y >= layout.height) return true;
        return layout.grid[x][y] == 'X';
    };

    if (isWall(gx, gy)) return "wall";

    int mask = 0;
    if (isWall(gx, gy - 1)) mask |= 1;
    if (isWall(gx + 1, gy - 1)) mask |= 2;
    if (isWall(gx + 1, gy)) mask |= 4;
    if (isWall(gx + 1, gy + 1)) mask |= 8;
    if (isWall(gx, gy + 1)) mask |= 16;
    if (isWall(gx - 1, gy + 1)) mask |= 32;
    if (isWall(gx - 1, gy)) mask |= 64;
    if (isWall(gx - 1, gy - 1)) mask |= 128;
    return WALL_SHAPE_BY_MASK[mask];
}

void DungeonGenerator::generateDungeon() {
    int mapWidth = levelsX * 64;
    int mapHeight = levelsY * 64;
    int layoutWidth = layout.width * 8;
    int layoutHeight = layout.height * 11;

    for (int gy = 0; gy < mapHeight; ++gy) {
        for (int gx = 0; gx < mapWidth; ++gx) {
            bool inside = gx < layoutWidth && gy < layoutHeight;
            uint16_t base = inside ? 0x80C : 0xA00;
            uint16_t tile = static_cast<uint16_t>(base + (gx & 3) + ((gy & 3) << 4));
            levels[gx / 64][gy / 64].setTile(gx % 64, gy % 64, tile);
        }
    }

    for (int gx = 0; gx < layout.width; ++gx) {
        for (int gy = 0; gy < layout.height; ++gy) {
            std::string shapeName = determineWallShape(gx, gy);
            if (shapeName != "path") {
                auto it = options.wallTemplates.find(shapeName);
                if (it == options.wallTemplates.end()) {
                    it = options.wallTemplates.find("wall");
                }
                if (it != options.wallTemplates.end()) {
                    const auto& templateTiles = it->second;
                    for (int subY = 0; subY < 11; ++subY) {
                        for (int subX = 0; subX < 8; ++subX) {
                            int idx = subY * 8 + subX;
                            if (idx >= static_cast<int>(templateTiles.size())) continue;
                            uint16_t tileVal = templateTiles[idx];
                            if (tileVal == 0) continue;
                            int totalTileX = gx * 8 + subX;
                            int totalTileY = gy * 11 + subY;
                            levels[totalTileX / 64][totalTileY / 64].setTile(totalTileX % 64, totalTileY % 64, tileVal);
                        }
                    }
                }
            }

            for (const auto& npc : options.npcs) {
                bool matchPlace = (npc.place == shapeName);
                if (matchPlace && (Utils::randomInt(100) <= npc.chance)) {
                    int totalTileX = gx * 8 + npc.x;
                    int totalTileY = gy * 11 + npc.y;
                    NpcDef placedNpc = npc;
                    placedNpc.x = totalTileX % 64;
                    placedNpc.y = totalTileY % 64;
                    levels[totalTileX / 64][totalTileY / 64].npcs.push_back(placedNpc);
                }
            }
        }
    }
}

void DungeonGenerator::writeLevels() {
    for (int y = 0; y < levelsY; ++y) {
        for (int x = 0; x < levelsX; ++x) {
            levels[x][y].saveToFile(options.levelStart, options.gmapName, options.addTestNpcs, options.wallTiles, options.groundTiles);
        }
    }
}

void DungeonGenerator::writeGMap() {
    gmap.width = levelsX;
    gmap.height = levelsY;
    gmap.generatedLastLevel = levels[levelsX - 1][levelsY - 1].name;
    gmap.saveToFile(options.gmapName + ".gmap");
}
