#ifndef LAYOUT_H
#define LAYOUT_H

#include "Options.h"
#include <vector>
#include <string>

class Layout {
public:
    int width = 0;
    int height = 0;
    std::vector<std::vector<char>> grid;

    bool generate(const Options& options);

private:
    bool loadFromFile(const std::string& filename);
    bool generateProceduralRoom(const std::string& roomFile, const std::string& statFile);
    bool generateStyle(const Options& options);
    void applyDoubleResolution();
};

#endif // LAYOUT_H
