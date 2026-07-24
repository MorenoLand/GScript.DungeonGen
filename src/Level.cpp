#include "Level.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

Level::Level() {
    for (int y = 0; y < 64; ++y) {
        for (int x = 0; x < 64; ++x) {
            tiles[y][x] = 0xFF9; // Default empty/wall tile
        }
    }
}

void Level::setTile(int x, int y, uint16_t tileVal) {
    if (x >= 0 && x < 64 && y >= 0 && y < 64) {
        tiles[y][x] = tileVal;
    }
}

std::string Level::formatLevelName(const std::string& prefix, int x, int y, int totalX, int totalY) {
    std::string xStr;
    if (totalX > 26) {
        char first = 'a' + ((x / 26) % 26);
        char second = 'a' + (x % 26);
        xStr += first;
        xStr += second;
    } else {
        xStr += static_cast<char>('a' + (x % 26));
    }

    int yNum = y + 1;
    std::ostringstream ySs;
    if (totalY > 99) {
        ySs << std::setw(3) << std::setfill('0') << yNum;
    } else if (totalY > 9) {
        ySs << std::setw(2) << std::setfill('0') << yNum;
    } else {
        ySs << yNum;
    }

    return prefix + xStr + "-" + ySs.str() + ".nw";
}

bool Level::saveToFile(const std::string& levelStart, const std::string& gmapName, bool addTestNpcs, const std::string& wallTiles, const std::string& groundTiles) const {
    std::ofstream file(name);
    if (!file.is_open()) {
        std::cerr << "Could not open level file for writing: " << name << std::endl;
        return false;
    }

    file << "GLEVNW01\n";

    // Write 64 tile lines
    for (int y = 0; y < 64; ++y) {
        file << "BOARD 0 " << y << " 64 0 ";
        for (int x = 0; x < 64; ++x) {
            uint16_t t = tiles[y][x];
            char c1 = Utils::getBase64Char(t & 0x3F);
            char c2 = Utils::getBase64Char((t >> 6) & 0x3F);
            file << c1 << c2;
        }
        file << "\n";
    }

    // Write Links
    for (const auto& link : links) {
        file << "LINK " << link.targetLevel << " "
             << link.x << " " << link.y << " "
             << link.w << " " << link.h << " "
             << link.newX << " " << link.newY << "\n";
    }

    // Write NPCs
    for (const auto& npc : npcs) {
        if (!npc.image.empty()) {
            file << "NPC " << npc.image << " " << npc.x << " " << npc.y << "\n";
        } else if (!npc.script.empty()) {
            file << "NPC - " << npc.x << " " << npc.y << "\n";
            std::ifstream sf(npc.script);
            if (sf.is_open()) {
                std::string sLine;
                while (std::getline(sf, sLine)) {
                    file << sLine << "\n";
                }
            }
        } else {
            file << "NPC - " << npc.x << " " << npc.y << "\n";
        }

        if (npc.underplayer) {
            file << "if (created) {\n  dontblock;\n  drawunderplayer;\n}\n";
        }
        file << "NPCEND\n";
    }

    // Write Test NPC block if ADDTESTNPCS is set
    if (addTestNpcs) {
        file << "NPC - 0 0\n"
             << "//#CLIENTSIDE\n"
             << "if (playerenters) {\n"
             << "  addtiledef tiles_inside.png," << levelStart << ",1;\n"
             << "  addtiledef2 " << wallTiles << "," << levelStart << ",1024,224;\n"
             << "  addtiledef2 " << groundTiles << "," << levelStart << ",1216,0;\n"
             << "  loadmap " << gmapName << ";\n"
             << "}\n"
             << "NPCEND\n";
    }

    return true;
}
