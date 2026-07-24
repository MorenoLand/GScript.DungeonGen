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

    // Style generators
    std::vector<std::vector<char>> generateOnion(int w, int h, int opt, int sym);
    std::vector<std::vector<char>> generateMaze(int w, int h, int sym);
    std::vector<std::vector<char>> generateSpiral(int w, int h, int sym);
    std::vector<std::vector<char>> generateRogue(int w, int h, int sym);
    std::vector<std::vector<char>> generateSnake(int w, int h);
    std::vector<std::vector<char>> generateSquareSpiral(int w, int h);

    void applySymmetry(int sym);
    void applyDoubleResolution();
};

#endif // LAYOUT_H
