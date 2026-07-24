#include "DungeonGenerator.h"
#include "Utils.h"
#include <iostream>
#include <ctime>
#include <cstdlib>

static const char* WALL_TYPE_NAMES[15] = {
    "wallse",  // 0
    "walle",   // 1
    "wallne",  // 2
    "walls",   // 3
    "wall",    // 4
    "walln",   // 5
    "wallsw",  // 6
    "wallw",   // 7
    "wallnw",  // 8
    "wallnw2", // 9
    "wallne2", // 10
    "wallsw2", // 11
    "wallse2", // 12
    "walld1",  // 13
    "walld2"   // 14
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
        std::srand(seed);
    } else {
        std::cout << "Seed loaded from file value:" << options.randomSeed << "\n";
        std::srand(options.randomSeed);
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
                l.newX = "64"; l.newY = "playery";
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
                l.newX = "playerx"; l.newY = "64";
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

int DungeonGenerator::determineWallShape(int gx, int gy) const {
    auto isWall = [this](int x, int y) -> bool {
        if (x < 0 || x >= layout.width || y < 0 || y >= layout.height) return true;
        return layout.grid[x][y] == 'X';
    };

    if (isWall(gx, gy)) return 4; // Default wall cell

    bool W  = isWall(gx, gy - 1);
    bool NW = isWall(gx - 1, gy - 1);
    bool N  = isWall(gx - 1, gy);
    bool NE = isWall(gx - 1, gy + 1);
    bool E  = isWall(gx, gy + 1);
    bool SE = isWall(gx + 1, gy + 1);
    bool S  = isWall(gx + 1, gy);
    bool SW = isWall(gx + 1, gy - 1);

    bool b1 = (NW && SW) || W;
    bool b3 = (NW && NE) || N;
    bool b2 = (NE && SE) || E;
    bool b4 = (SE && SW) || S;

    int sumB = b1 + b2 + b3 + b4;
    int sumDiag = NW + SW + NE + SE;

    if (sumB >= 3 || (b1 && b2) || (b3 && b4)) return 4;

    if (sumB == 2) {
        if (b1 && b3) return 9;
        if (b3 && b2) return 10;
        if (b2 && b4) return 12;
        return 11;
    }

    if (sumB == 1) {
        if (b1 || SE) return 7;
        if (b3 || SW) return 5;
        if (b2 || NW) return 1;
        return 3;
    }

    if (sumDiag == 2) {
        if (NW && SE) return 13;
        if (SW && NE) return 14;
    }

    if (sumDiag == 1) {
        if (NW) return 8;
        if (NE) return 2;
        if (SE) return 0;
        return 6;
    }

    return 14;
}

void DungeonGenerator::generateDungeon() {
    for (int gx = 0; gx < layout.width; ++gx) {
        for (int gy = 0; gy < layout.height; ++gy) {
            int wallShape = determineWallShape(gx, gy);
            std::string shapeName = (wallShape >= 0 && wallShape < 15) ? WALL_TYPE_NAMES[wallShape] : "wall";

            auto it = options.wallTemplates.find(shapeName);
            if (it == options.wallTemplates.end()) {
                it = options.wallTemplates.find("wall");
            }

            const auto& templateTiles = (it != options.wallTemplates.end()) ? it->second : std::vector<uint16_t>();

            // Each grid cell is 8 tiles wide x 11 tiles high
            for (int subY = 0; subY < 11; ++subY) {
                for (int subX = 0; subX < 8; ++subX) {
                    int totalTileX = gx * 8 + subX;
                    int totalTileY = gy * 11 + subY;

                    int lvlX = totalTileX / 64;
                    int lvlY = totalTileY / 64;
                    int localTileX = totalTileX % 64;
                    int localTileY = totalTileY % 64;

                    if (lvlX < levelsX && lvlY < levelsY) {
                        uint16_t tileVal = 0xFF9;
                        int idx = subY * 8 + subX;
                        if (idx < static_cast<int>(templateTiles.size())) {
                            tileVal = templateTiles[idx];
                        }
                        levels[lvlX][lvlY].setTile(localTileX, localTileY, tileVal);
                    }
                }
            }

            // Place NPCs
            for (const auto& npc : options.npcs) {
                bool matchPlace = (npc.place == "path" && layout.grid[gx][gy] != 'X') ||
                                 (npc.place == "wall" && layout.grid[gx][gy] == 'X') ||
                                 (npc.place == shapeName);
                if (matchPlace && (Utils::randomInt(100) < npc.chance)) {
                    int totalTileX = gx * 8 + npc.x;
                    int totalTileY = gy * 11 + npc.y;
                    int lvlX = totalTileX / 64;
                    int lvlY = totalTileY / 64;
                    int localTileX = totalTileX % 64;
                    int localTileY = totalTileY % 64;

                    if (lvlX < levelsX && lvlY < levelsY) {
                        NpcDef placedNpc = npc;
                        placedNpc.x = localTileX;
                        placedNpc.y = localTileY;
                        levels[lvlX][lvlY].npcs.push_back(placedNpc);
                    }
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
