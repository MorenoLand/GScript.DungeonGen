#ifndef LEVEL_H
#define LEVEL_H

#include "Options.h"
#include <string>
#include <vector>

struct Link {
    std::string targetLevel;
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    std::string newX;
    std::string newY;
};

class Level {
public:
    std::string name;
    int xIndex = 0;
    int yIndex = 0;

    uint16_t tiles[64][64];
    std::vector<Link> links;
    std::vector<NpcDef> npcs;

    Level();
    void setTile(int x, int y, uint16_t tileVal);
    bool saveToFile(const std::string& levelStart, const std::string& gmapName, bool addTestNpcs, const std::string& wallTiles, const std::string& groundTiles) const;

    static std::string formatLevelName(const std::string& prefix, int x, int y, int totalX, int totalY);
};

#endif // LEVEL_H
