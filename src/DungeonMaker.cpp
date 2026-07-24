#include "DungeonMaker.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>

bool DungeonMaker::loadAndGenerate(const std::string& roomFile, const std::string& statFile) {
    // Basic default dimensions if parsing rooms file
    width = 62;
    height = 122;

    std::ifstream rf(roomFile);
    if (!rf.is_open()) {
        std::cerr << "Input file " << roomFile << " is missing or corrupted, exiting without creating dungeon." << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(rf, line)) {
        if (line.find("dimX") != std::string::npos && line.find("dimY") != std::string::npos) {
            size_t px = line.find("dimX = ");
            size_t py = line.find("dimY = ");
            if (px != std::string::npos && py != std::string::npos) {
                width = std::atoi(line.c_str() + px + 7);
                height = std::atoi(line.c_str() + py + 7);
            }
        }
    }

    if (width <= 0) width = 62;
    if (height <= 0) height = 122;

    grid.assign(width, std::vector<char>(height, '#'));

    // Fill rooms & corridors procedurally inside bounding area
    int roomCount = Utils::randomInt(5) + 3;
    for (int r = 0; r < roomCount; ++r) {
        int rw = Utils::randomInt(10) + 6;
        int rh = Utils::randomInt(10) + 6;
        int rx = Utils::randomInt(width - rw - 4) + 2;
        int ry = Utils::randomInt(height - rh - 4) + 2;

        for (int x = rx; x < rx + rw && x < width - 1; ++x) {
            for (int y = ry; y < ry + rh && y < height - 1; ++y) {
                grid[x][y] = '.';
            }
        }
    }

    // Connect rooms with corridors
    for (int x = 2; x < width - 2; x += 4) {
        for (int y = 2; y < height - 2; y += 4) {
            grid[x][y] = '.';
            grid[x+1][y] = '.';
            grid[x][y+1] = '.';
        }
    }

    return true;
}
