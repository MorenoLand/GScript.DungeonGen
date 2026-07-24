#include "Layout.h"
#include "DungeonMaker.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <cstdlib>

bool Layout::generate(const Options& options) {
    if (!options.layoutFile.empty()) {
        if (loadFromFile(options.layoutFile)) {
            if (options.doubleRes) applyDoubleResolution();
            return true;
        }
    }

    if (!options.roomFile.empty()) {
        if (generateProceduralRoom(options.roomFile, options.statFile)) {
            if (options.doubleRes) applyDoubleResolution();
            return true;
        }
    }

    if (!generateStyle(options)) {
        std::cerr << "Cannot make dungeon layout" << std::endl;
        return false;
    }

    if (options.doubleRes) applyDoubleResolution();
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
    int w = (options.layoutWidth >= 4) ? options.layoutWidth : (Utils::randomInt(25) + 15);
    int h = (options.layoutHeight >= 4) ? options.layoutHeight : (Utils::randomInt(25) + 15);

    std::string style = options.layoutStyle;
    if (style.find("onion") != std::string::npos) {
        grid = generateOnion(w, h, options.layoutOption, options.layoutSymmetry);
    } else if (style.find("maze") != std::string::npos) {
        grid = generateMaze(w, h, options.layoutSymmetry);
    } else if (style.find("spiral") != std::string::npos) {
        grid = generateSpiral(w, h, options.layoutSymmetry);
    } else if (style.find("rogue") != std::string::npos) {
        grid = generateRogue(w, h, options.layoutSymmetry);
    } else if (style.find("snake") != std::string::npos) {
        grid = generateSnake(w, h);
    } else if (style.find("squarespiral") != std::string::npos) {
        grid = generateSquareSpiral(w, h);
    } else {
        // Random default style
        int r = Utils::randomInt(5);
        if (r == 0) grid = generateMaze(w, h, options.layoutSymmetry);
        else if (r == 1) grid = generateOnion(w, h, options.layoutOption, options.layoutSymmetry);
        else if (r == 2) grid = generateSpiral(w, h, options.layoutSymmetry);
        else if (r == 3) grid = generateRogue(w, h, options.layoutSymmetry);
        else grid = generateSnake(w, h);
    }

    width = static_cast<int>(grid.size());
    height = (width > 0) ? static_cast<int>(grid[0].size()) : 0;
    return true;
}

std::vector<std::vector<char>> Layout::generateOnion(int w, int h, int opt, int sym) {
    std::vector<std::vector<char>> g(w, std::vector<char>(h, '#'));
    int ringCount = std::min(w, h) / 4;
    for (int r = 0; r < ringCount; ++r) {
        int x0 = r * 2 + 1;
        int y0 = r * 2 + 1;
        int x1 = w - 1 - (r * 2 + 1);
        int y1 = h - 1 - (r * 2 + 1);
        if (x0 >= x1 || y0 >= y1) break;

        for (int x = x0; x <= x1; ++x) { g[x][y0] = '.'; g[x][y1] = '.'; }
        for (int y = y0; y <= y1; ++y) { g[x0][y] = '.'; g[x1][y] = '.'; }

        // Openings
        g[(x0 + x1) / 2][y0] = '.';
        g[x0][(y0 + y1) / 2] = '.';
    }
    return g;
}

std::vector<std::vector<char>> Layout::generateMaze(int w, int h, int sym) {
    std::vector<std::vector<char>> g(w, std::vector<char>(h, '#'));
    for (int x = 1; x < w - 1; ++x) {
        for (int y = 1; y < h - 1; ++y) {
            if ((x % 2 == 1) || (y % 2 == 1)) {
                g[x][y] = '.';
            }
        }
    }
    return g;
}

std::vector<std::vector<char>> Layout::generateSpiral(int w, int h, int sym) {
    std::vector<std::vector<char>> g(w, std::vector<char>(h, '#'));
    float cx = w / 2.0f;
    float cy = h / 2.0f;
    float maxR = std::min(cx, cy) - 2.0f;
    float angle = 0.0f;
    while (angle < 4.0f * 3.14159f) {
        float r = (angle / (4.0f * 3.14159f)) * maxR;
        int px = static_cast<int>(cx + r * std::cos(angle));
        int py = static_cast<int>(cy + r * std::sin(angle));
        if (px >= 1 && px < w - 1 && py >= 1 && py < h - 1) {
            g[px][py] = '.';
        }
        angle += 0.1f;
    }
    return g;
}

std::vector<std::vector<char>> Layout::generateRogue(int w, int h, int sym) {
    std::vector<std::vector<char>> g(w, std::vector<char>(h, '#'));
    int roomW = (w - 4) / 3;
    int roomH = (h - 4) / 3;
    for (int rx = 0; rx < 3; ++rx) {
        for (int ry = 0; ry < 3; ++ry) {
            int x0 = 1 + rx * (roomW + 1);
            int y0 = 1 + ry * (roomH + 1);
            for (int x = x0; x < x0 + roomW - 1 && x < w - 1; ++x) {
                for (int y = y0; y < y0 + roomH - 1 && y < h - 1; ++y) {
                    g[x][y] = '.';
                }
            }
        }
    }
    return g;
}

std::vector<std::vector<char>> Layout::generateSnake(int w, int h) {
    std::vector<std::vector<char>> g(w, std::vector<char>(h, '#'));
    bool leftToRight = true;
    for (int y = 1; y < h - 1; y += 2) {
        for (int x = 1; x < w - 1; ++x) {
            g[x][y] = '.';
        }
        if (y + 1 < h - 1) {
            int turnX = leftToRight ? (w - 2) : 1;
            g[turnX][y + 1] = '.';
        }
        leftToRight = !leftToRight;
    }
    return g;
}

std::vector<std::vector<char>> Layout::generateSquareSpiral(int w, int h) {
    return generateOnion(w, h, 0, 0);
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
