#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <vector>
#include <map>

struct NpcDef {
    std::string script;
    std::string image;
    bool blocking = false;
    bool underplayer = false;
    int chance = 100;
    std::string place;
    int x = 0;
    int y = 0;
};

class Options {
public:
    std::string levelStart = "testhouse_";
    std::string gmapName = "testhouse";
    std::string wallTiles = "tiles_wallshorror.png";
    std::string groundTiles = "tiles_groundhorror.png";
    bool addTestNpcs = false;
    bool doubleRes = false;
    unsigned int randomSeed = 0;

    std::string layoutFile;
    std::string roomFile;
    std::string statFile;

    int layoutWidth = 0;
    int layoutHeight = 0;
    std::string layoutStyle;
    int layoutOption = 0;
    int layoutSymmetry = 0;
    int layoutOrientation = 0;

    // Wall tile templates: wall, walln, walle, walls, wallw, wallne, wallnw, wallse, wallsw, wallne2, wallnw2, wallse2, wallsw2, walld1, walld2
    std::map<std::string, std::vector<uint16_t>> wallTemplates;
    std::vector<NpcDef> npcs;

    bool loadFromFile(const std::string& filename);
};

#endif // OPTIONS_H
