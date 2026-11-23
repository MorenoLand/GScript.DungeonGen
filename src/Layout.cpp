#include "Layout.h"
#include "DungeonMaker.h"
#include "LayoutStyle.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <cstdlib>

bool Layout::generate(const Options& options) {
    if (!options.layoutFile.empty()) {
        if (loadFromFile(options.layoutFile)) {
            return true;
        }
    }

    if (!options.roomFile.empty()) {
        if (generateProceduralRoom(options.roomFile, options.statFile)) {
            return true;
        }
    }

    if (!generateStyle(options)) {
        std::cerr << "Cannot make dungeon layout" << std::endl;
        return false;
    }

    return true;
}

bool Layout::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::vector<std::string> lines;
    std::string line;
    size_t maxW = 0;
    while (std::getline(file, line)) {
        // Strip trailing \r
        if (!line.empty() && line.back() == '\r') line.pop_back();
        lines.push_back(line);
        if (line.length() > maxW) maxW = line.length();
    }

    if (lines.empty() || maxW == 0) return false;

    width = static_cast<int>(maxW);
    height = static_cast<int>(lines.size());
    grid.assign(width, std::vector<char>(height, '.'));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < static_cast<int>(lines[y].length()); ++x) {
            char c = lines[y][x];
            if (c == '#' || c == 'X') {
                grid[x][y] = 'X';
            } else {
                grid[x][y] = '.';
            }
        }
    }

    return true;
}

bool Layout::generateProceduralRoom(const std::string& roomFile, const std::string& statFile) {
    DungeonMaker maker;
    if (!maker.loadAndGenerate(roomFile, statFile)) return false;
    width = maker.width;
    height = maker.height;
    grid = maker.grid;
    return true;
}

bool Layout::generateStyle(const Options& options) {
    int w = options.layoutWidth;
    int h = options.layoutHeight;
    if (options.doubleRes) {
        w /= 2;
        h /= 2;
    }
    int sym = options.layoutSymmetry;
    if (sym == 1) {
        if (w < 4) w = Utils::randomInt(25) + 15;
        if (h < 4) h = Utils::randomInt(25) + 15;
    } else {
        if (w < 15) w = Utils::randomInt(25) + 15;
        if (h < 15) h = Utils::randomInt(25) + 15;
    }
    if (sym == 0) {
        sym = Utils::randomInt(4) + 1;
        if (sym == 3 || sym == 4) h = h / 2 + 1;
        if (sym == 2 || sym == 4) w = w / 2 + 1;
    } else {
        if (sym == 3 || sym == 4) h = h / 2 + 1;
        if (sym == 2 || sym == 4) w = w / 2 + 1;
    }

    std::string style = options.layoutStyle;
    LayoutStyle::Grid g;
    int styleNo = 0;
    if (style.find("onion") != std::string::npos) {
        g = LayoutStyle::onion(w, h, static_cast<unsigned int>(options.layoutOption), 0);
        styleNo = 1;
        if (Utils::randomInt(3) == 0 && (options.layoutOption & 0x40) == 0) {
            LayoutStyle::onionFinishing(g, w, h);
        }
    }
    if (style.find("maze") != std::string::npos) {
        g = LayoutStyle::maze(w, h, options.layoutOption);
        styleNo = 2;
    }
    if (style.find("spiral") != std::string::npos) {
        g = LayoutStyle::spiral(w, h, static_cast<unsigned int>(options.layoutOption));
        styleNo = 3;
    }
    if (style.find("rogue") != std::string::npos) {
        g = LayoutStyle::rogue(w, h, options.layoutOption);
        styleNo = 4;
    }
    if (style.find("snake") != std::string::npos) {
        g = LayoutStyle::snake(w, h);
        styleNo = 5;
    }
    if (style.find("squarespiral") != std::string::npos) {
        g = LayoutStyle::squareSpiral(w, h);
        styleNo = 6;
    }
    if (g.empty()) {
        int r = Utils::randomInt(6);
        if (r == 0) { g = LayoutStyle::maze(w, h, Utils::randomInt(2)); styleNo = 2; }
        else if (r == 1) { g = LayoutStyle::onion(w, h, static_cast<unsigned int>(options.layoutOption), 0); styleNo = 1; }
        else if (r == 2) { g = LayoutStyle::spiral(w, h, static_cast<unsigned int>(options.layoutOption)); styleNo = 3; }
        else if (r == 3) { g = LayoutStyle::rogue(w, h, options.layoutOption); styleNo = 4; }
        else if (r == 4) { g = LayoutStyle::snake(w, h); styleNo = 5; }
        else { g = LayoutStyle::squareSpiral(w, h); styleNo = 6; }
    }

    LayoutStyle::applySymmetry(g, sym, w, h, styleNo);
    if (options.doubleRes) {
        g = LayoutStyle::doubleScale(g, w, h);
        w = w * 2 - 1;
        h = h * 2 - 1;
    }
    int rotation = Utils::randomInt(4);
    g = LayoutStyle::rotate(g, rotation, w, h);
    LayoutStyle::orphanWallCleanup(g, w, h);

    if (g.empty()) return false;
    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            if (g[x][y] == '#') g[x][y] = 'X';
            else if (g[x][y] == 0) g[x][y] = '.';
        }
    }
    grid = g;
    width = w;
    height = h;
    return true;
}

void Layout::applyDoubleResolution() {
    int nw = width * 2;
    int nh = height * 2;
    std::vector<std::vector<char>> ng(nw, std::vector<char>(nh, '.'));

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            char val = grid[x][y];
            ng[x * 2][y * 2] = val;
            ng[x * 2 + 1][y * 2] = val;
            ng[x * 2][y * 2 + 1] = val;
            ng[x * 2 + 1][y * 2 + 1] = val;
        }
    }
    width = nw;
    height = nh;
    grid = ng;
}
