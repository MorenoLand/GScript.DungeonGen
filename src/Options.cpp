#include "Options.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <sstream>

bool Options::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open options file: " << filename << std::endl;
        return false;
    }

    std::string line;
    bool isFirstLine = true;

    while (std::getline(file, line)) {
        line = Utils::trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (isFirstLine) {
            if (line != "GDUNG001") {
                std::cerr << "Invalid header in options file, expected GDUNG001, got: " << line << std::endl;
                return false;
            }
            isFirstLine = false;
            continue;
        }

        std::stringstream ss(line);
        std::string key;
        ss >> key;

        if (key == "LEVELSTART") {
            ss >> levelStart;
        } else if (key == "GMAP") {
            ss >> gmapName;
        } else if (key == "WALLTILES") {
            ss >> wallTiles;
        } else if (key == "GROUNDTILES") {
            ss >> groundTiles;
        } else if (key == "ADDTESTNPCS") {
            addTestNpcs = true;
        } else if (key == "DOUBLE") {
            doubleRes = true;
        } else if (key == "RANDOMSEED") {
            ss >> randomSeed;
        } else if (key == "LAYOUTFILE") {
            ss >> layoutFile;
        } else if (key == "ROOMFILE") {
            ss >> roomFile;
        } else if (key == "STATFILE") {
            ss >> statFile;
        } else if (key == "LAYOUTWIDTH") {
            ss >> layoutWidth;
        } else if (key == "LAYOUTHEIGHT") {
            ss >> layoutHeight;
        } else if (key == "LAYOUTSTYLE") {
            ss >> layoutStyle;
        } else if (key == "LAYOUTOPTION") {
            ss >> layoutOption;
        } else if (key == "LAYOUTSYMMETRY") {
            ss >> layoutSymmetry;
        } else if (key == "LAYOUTORIENTATION") {
            ss >> layoutOrientation;
        } else if (key == "TILES") {
            std::string rest;
            std::getline(ss, rest);
            rest = Utils::trim(rest);
            size_t eqPos = rest.find('=');
            if (eqPos != std::string::npos) {
                std::string wallType = Utils::trim(rest.substr(0, eqPos));
                std::string tilesStr = rest.substr(eqPos + 1);
                std::vector<std::string> tileTokens = Utils::split(tilesStr, ',');
                std::vector<uint16_t> tiles;
                for (const auto& tok : tileTokens) {
                    tiles.push_back(static_cast<uint16_t>(Utils::parseTileVal(tok)));
                }
                wallTemplates[wallType] = tiles;
            }
        } else if (key == "NPC") {
            std::string rest;
            std::getline(ss, rest);
            rest = Utils::trim(rest);
            std::vector<std::string> attrTokens = Utils::split(rest, ',');
            NpcDef npc;
            for (const auto& attrTok : attrTokens) {
                size_t eqPos = attrTok.find('=');
                if (eqPos != std::string::npos) {
                    std::string attrKey = Utils::trim(attrTok.substr(0, eqPos));
                    std::string attrVal = Utils::trim(attrTok.substr(eqPos + 1));
                    if (attrKey == "script") npc.script = attrVal;
                    else if (attrKey == "image") npc.image = attrVal;
                    else if (attrKey == "blocking") npc.blocking = (attrVal == "true");
                    else if (attrKey == "underplayer") npc.underplayer = (attrVal == "true");
                    else if (attrKey == "chance") {
                        if (!attrVal.empty() && attrVal.back() == '%') attrVal.pop_back();
                        npc.chance = std::atoi(attrVal.c_str());
                    } else if (attrKey == "place") npc.place = attrVal;
                    else if (attrKey == "x") npc.x = std::atoi(attrVal.c_str());
                    else if (attrKey == "y") npc.y = std::atoi(attrVal.c_str());
                }
            }
            npcs.push_back(npc);
        }
    }

    return true;
}
