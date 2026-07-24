#ifndef DUNGEONGENERATOR_H
#define DUNGEONGENERATOR_H

#include "Options.h"
#include "Layout.h"
#include "Level.h"
#include "GMap.h"
#include <string>
#include <vector>

class DungeonGenerator {
public:
    Options options;
    Layout layout;
    int levelsX = 0;
    int levelsY = 0;
    std::vector<std::vector<Level>> levels;
    GMap gmap;

    void run(const std::string& optionsFilename);

private:
    bool loadOptions(const std::string& optionsFilename);
    bool loadDungeonDefinition();
    void createLevels();
    void generateDungeon();
    void writeLevels();
    void writeGMap();

    int determineWallShape(int gx, int gy) const;
};

#endif // DUNGEONGENERATOR_H
