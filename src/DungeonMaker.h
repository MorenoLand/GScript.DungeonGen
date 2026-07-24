#ifndef DUNGEONMAKER_H
#define DUNGEONMAKER_H

#include <string>
#include <vector>

class DungeonMaker {
public:
    int width = 0;
    int height = 0;
    std::vector<std::vector<char>> grid;

    bool loadAndGenerate(const std::string& roomFile, const std::string& statFile);
};

#endif // DUNGEONMAKER_H
