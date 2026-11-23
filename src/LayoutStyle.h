#ifndef LAYOUTSTYLE_H
#define LAYOUTSTYLE_H

#include <vector>

namespace LayoutStyle {

using Grid = std::vector<std::vector<char>>;

Grid maze(int width, int height, int option);
Grid onion(int width, int height, unsigned int flags, int option);
Grid spiral(int width, int height, unsigned int flags);
Grid rogue(int width, int height, int option);
Grid snake(int width, int height);
Grid squareSpiral(int width, int height);
Grid rotate(const Grid& grid, int mode, int& width, int& height);
void orphanWallCleanup(Grid& grid, int width, int height);
void onionFinishing(Grid& grid, int width, int height);
Grid doubleScale(const Grid& grid, int width, int height);
void applySymmetry(Grid& grid, int symmetry, int& width, int& height, int styleNumber);

}

#endif
