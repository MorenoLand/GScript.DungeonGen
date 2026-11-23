#include "LayoutStyle.h"
#include "Utils.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <vector>

namespace LayoutStyle {
namespace {

Grid makeGrid(int width, int height) {
    return Grid(width, std::vector<char>(height, 0));
}

int rnd() {
    return Utils::randomValue();
}

struct MazeState {
    Grid* grid = nullptr;
    int width = 0;
    int height = 0;
    std::vector<int> xs;
    std::vector<int> ys;
    int count = 0;
};

void mazePick(MazeState& s, int& x, int& y) {
    int i = rnd() % s.count;
    x = s.xs[i];
    y = s.ys[i];
    s.xs[i] = s.xs[s.count - 1];
    s.ys[i] = s.ys[s.count - 1];
    s.count--;
}

int mazeTry(MazeState& s, int cx, int cy, int& nx, int& ny) {
    int options[4];
    int n = 0;
    Grid& g = *s.grid;
    bool blocked = false;
    if (cy < s.height - 3 && cx > 3 && cx < s.width - 3) {
        for (int i = 1; i < 5 && !blocked; ++i) {
            for (int j = -2; j < 3; ++j) {
                if (g[cx + j][cy + i] != 0) { blocked = true; break; }
            }
        }
        if (!blocked) options[n++] = 1;
    }
    blocked = false;
    if (cy > 3 && cx > 3 && cx < s.width - 3) {
        for (int i = -4; i < 0 && !blocked; ++i) {
            for (int j = -2; j < 3; ++j) {
                if (g[cx + j][cy + i] != 0) { blocked = true; break; }
            }
        }
        if (!blocked) options[n++] = 2;
    }
    blocked = false;
    if (cx < s.width - 3 && cy > 3 && cy < s.height - 3) {
        for (int j = 1; j < 5 && !blocked; ++j) {
            for (int i = -2; i < 3; ++i) {
                if (g[cx + j][cy + i] != 0) { blocked = true; break; }
            }
        }
        if (!blocked) options[n++] = 3;
    }
    blocked = false;
    if (cx > 3 && cy > 3 && cy < s.height - 3) {
        for (int i = -4; i < 0 && !blocked; ++i) {
            for (int j = -2; j < 3; ++j) {
                if (g[cx + j][cy + i] != 0) { blocked = true; break; }
            }
        }
        if (!blocked) options[n++] = 4;
    }
    if (n == 0) return -1;
    int choice = options[n < 2 ? 0 : rnd() % n];
    if (choice == 1) { nx = cx; ny = cy + 1; }
    else if (choice == 2) { nx = cx; ny = cy - 1; }
    else if (choice == 3) { nx = cx + 1; ny = cy; }
    else { nx = cx - 1; ny = cy; }
    return 1;
}

void mazeCarveA(MazeState& s, int x, int y) {
    s.grid->at(x)[y] = '#';
    if ((rnd() & 3) != 0 && s.count > 0) {
        int nx, ny;
        mazePick(s, nx, ny);
        mazeCarveA(s, nx, ny);
    }
    for (;;) {
        int nx, ny;
        if (mazeTry(s, x, y, nx, ny) == -1) break;
        mazeCarveA(s, nx, ny);
    }
}

void mazeCarveB(MazeState& s, int x, int y) {
    s.grid->at(x)[y] = '#';
    if ((rnd() & 3) != 0 && s.count > 0) {
        int nx, ny;
        mazePick(s, nx, ny);
        mazeCarveB(s, nx, ny);
    }
    int nx, ny;
    if (mazeTry(s, x, y, nx, ny) != -1) mazeCarveB(s, nx, ny);
}

int neighborMask(const Grid& g, int x, int y, int width, int height) {
    int m = 0;
    if (x > 0 && g[x - 1][y] != 0) m |= 1;
    if (x < width - 1 && g[x + 1][y] != 0) m |= 2;
    if (y > 0 && g[x][y - 1] != 0) m |= 4;
    if (y < height - 1 && g[x][y + 1] != 0) m |= 8;
    return m;
}

int corridorOpen(const Grid& g, int x, int y, int dir, int width, int height) {
    if (x == 0 || x == width - 1 || y == 0 || y == height - 1) return -1;
    if (g[x][y] != 0) return -1;
    int distance = 0;
    if (dir == 0) {
        int i = x;
        while (--i > 0) {
            int m = neighborMask(g, i, y, width, height);
            if (m == 1) break;
            if (m != 0) return -1;
            distance++;
        }
        for (;;) {
            int j = x + 1;
            if (j >= width - 1) return distance;
            int m = neighborMask(g, j, y, width, height);
            if (m == 2) return distance;
            if (m != 0) break;
            distance++;
            x = j;
        }
    } else {
        int i = y;
        while (--i > 0) {
            int m = neighborMask(g, x, i, width, height);
            if (m == 4) break;
            if (m != 0) return -1;
            distance++;
        }
        for (;;) {
            int j = y + 1;
            if (j >= height - 1) return distance;
            int m = neighborMask(g, x, j, width, height);
            if (m == 8) return distance;
            if (m != 0) break;
            distance++;
            y = j;
        }
    }
    return -1;
}

void carveDoor(Grid& g, int x, int y, int dir) {
    g[x][y] = 'D';
    if (dir == 0) {
        int i = x;
        while (--i >= 0 && g[i][y] == 0) g[i][y] = '#';
        i = x + 1;
        while (i < static_cast<int>(g.size()) && g[i][y] == 0) { g[i][y] = '#'; x = i; i++; }
    } else {
        int i = y;
        while (--i >= 0 && g[x][i] == 0) g[x][i] = '#';
        i = y + 1;
        while (i < static_cast<int>(g[x].size()) && g[x][i] == 0) { g[x][i] = '#'; y = i; i++; }
    }
}

void onionDrawRings(Grid& g, const float* posX, const float* posY, int rings) {
    for (int i = 0; i < rings; ++i) {
        float top = posY[i];
        float bottom = posY[rings * 2 - 1 - i];
        int x = static_cast<int>(posX[i]);
        while (x <= static_cast<int>(posX[rings * 2 - 1 - i])) {
            g[x][static_cast<int>(top)] = '#';
            g[x][static_cast<int>(bottom)] = '#';
            x++;
        }
        float left = posX[i];
        float right = posX[rings * 2 - 1 - i];
        int y = static_cast<int>(posY[i]);
        while (y <= static_cast<int>(posY[rings * 2 - 1 - i])) {
            g[static_cast<int>(left)][y] = '#';
            g[static_cast<int>(right)][y] = '#';
            y++;
        }
    }
}

void onionPlaceDoors(Grid& g, const float* posX, const float* posY, int rings, unsigned int flags) {
    int sides = 4;
    if (flags & 4) sides = 3;
    if (flags & 8) sides = 2;
    int side = 0;
    if (rings > 0) {
        side = rnd() % sides + 1;
        for (int i = 0; i < rings; ++i) {
            if ((flags & 2) == 0) side = rnd() % sides + 1;
            int x = 0, y = 0;
            if ((flags & 2) == 0) {
                int endX = static_cast<int>(posX[rings * 2 - 1 - i]);
                int endY = static_cast<int>(posY[rings * 2 - 1 - i]);
                if (side == 2) {
                    int span = static_cast<int>(posX[rings * 2 - 1 - i] - posX[i]) - 1;
                    float v = static_cast<float>(posX[i]);
                    if (span >= 1) v += static_cast<float>(rnd() % span);
                    x = static_cast<int>(v + 1.0f);
                    y = static_cast<int>(posY[i]);
                } else if (side < 3) {
                    int span = static_cast<int>(posY[rings * 2 - 1 - i] - posY[i]) - 1;
                    float v = static_cast<float>(posY[i]);
                    if (span >= 1) v += static_cast<float>(rnd() % span);
                    x = static_cast<int>(posX[i]);
                    y = static_cast<int>(v + 1.0f);
                } else if (side == 3) {
                    int span = static_cast<int>(posY[rings * 2 - 1 - i] - posY[i]) - 1;
                    float v = static_cast<float>(posY[i]);
                    if (span >= 1) v += static_cast<float>(rnd() % span);
                    x = endX;
                    y = static_cast<int>(v + 1.0f);
                } else if (side == 4) {
                    int span = static_cast<int>(posX[rings * 2 - 1 - i] - posX[i]) - 1;
                    float v = static_cast<float>(posX[i]);
                    if (span >= 1) v += static_cast<float>(rnd() % span);
                    x = static_cast<int>(v + 1.0f);
                    y = endY;
                }
            } else {
                int midX = static_cast<int>((static_cast<double>(posX[i]) + static_cast<double>(posX[rings * 2 - 1 - i])) / 2.0);
                int midY = static_cast<int>((static_cast<double>(posY[i]) + static_cast<double>(posY[rings * 2 - 1 - i])) / 2.0);
                if (side == 2) { x = midX; y = static_cast<int>(posY[i]); }
                else if (side < 3) { x = static_cast<int>(posX[i]); y = midY; }
                else if (side == 3) { x = static_cast<int>(posX[rings * 2 - 1 - i]); y = midY; }
                else if (side == 4) { x = midX; y = static_cast<int>(posY[rings * 2 - 1 - i]); }
            }
            if (x >= 0 && x < static_cast<int>(g.size()) && y >= 0 && y < static_cast<int>(g[x].size())) {
                g[x][y] = (flags & 0x100) ? '#' : 'D';
            }
        }
        int last = rings - 1;
        int cx = static_cast<int>((static_cast<double>(posX[last]) + static_cast<double>(posX[rings * 2 - 1 - last])) / 2.0);
        int cy = static_cast<int>((static_cast<double>(posY[last]) + static_cast<double>(posY[rings * 2 - 1 - last])) / 2.0);
        if (cx >= 0 && cx < static_cast<int>(g.size()) && cy >= 0 && cy < static_cast<int>(g[cx].size())) {
            g[cx][cy] = 'C';
        }
    }
}

void onionVariant(Grid& g, int width, int height, unsigned int flags, int option, int mode) {
    int smallest = std::min(width, height);
    int rings = (smallest - 2) / 5;
    if (rings == 0) return;
    if (rings < option) option = rings;
    if (option == 0) option = rnd() % rings + 1;
    int count = option * 2;
    std::vector<float> posX(count, 0.0f);
    std::vector<float> posY(count, 0.0f);
    if (flags & 0x10) {
        int spanX = width - (option * 6 + 2) + 1;
        int spanY = height - (option * 6 + 2) + 1;
        if (mode == 1) spanY = height - (option * 3 + 2) + 1;
        if (mode == 2) { spanX = width - (option * 3 + 2) + 1; spanY = height - (option * 3 + 2) + 1; }
        for (int i = 0; i < count; ++i) {
            float stepX = 2.0f;
            float stepY = 2.0f;
            if (spanX > 0) {
                int a = rnd() % spanX, b = rnd() % spanX, c = rnd() % spanX;
                stepX = static_cast<float>((a + b + c) / 3 + 2);
            }
            if (spanY > 0) {
                int a = rnd() % spanY, b = rnd() % spanY, c = rnd() % spanY;
                stepY = static_cast<float>((a + b + c) / 3 + 2);
            }
            posX[i] = (i == 0) ? stepX : posX[i - 1] + stepX;
            if (mode == 1) {
                if (i < option) posY[i] = (i == 0) ? stepY : posY[i - 1] + stepY;
                else posY[i] = static_cast<float>(height - 1);
            } else if (mode == 2) {
                if (i < option) posX[i] = (i == 0) ? stepX : posX[i - 1] + stepX;
                else posX[i] = static_cast<float>(width - 1);
                if (i < option) posY[i] = (i == 0) ? stepY : posY[i - 1] + stepY;
                else posY[i] = static_cast<float>(height - 1);
            } else {
                posY[i] = (i == 0) ? stepY : posY[i - 1] + stepY;
            }
            spanX = static_cast<int>(static_cast<float>(spanX) - (stepX - 2.0f));
            spanY = static_cast<int>(static_cast<float>(spanY) - (stepY - 2.0f));
        }
        if (mode == 2) {
            for (int i = option; i < count; ++i) posX[i] = static_cast<float>(width - 1);
        }
    } else {
        float stepX = static_cast<float>((static_cast<double>(width) - 2.0) / static_cast<double>(count + 1));
        float stepY = static_cast<float>((static_cast<double>(height) - 2.0) / static_cast<double>(count + 1));
        if (mode == 1) stepY = static_cast<float>((static_cast<double>(height) - 2.0) / static_cast<double>(option + 1));
        if (mode == 2) {
            stepX = static_cast<float>((static_cast<double>(width) - 2.0) / static_cast<double>(option + 1));
            stepY = static_cast<float>((static_cast<double>(height) - 2.0) / static_cast<double>(option + 1));
        }
        posX[0] = stepX;
        posY[0] = stepY;
        for (int i = 1; i < count; ++i) {
            if (mode == 2) {
                if (i < option) posX[i] = posX[i - 1] + stepX;
                else posX[i] = static_cast<float>(width - 1);
                if (i < option) posY[i] = posY[i - 1] + stepY;
                else posY[i] = static_cast<float>(height - 1);
            } else if (mode == 1) {
                posX[i] = posX[i - 1] + stepX;
                if (i < option) posY[i] = posY[i - 1] + stepY;
                else posY[i] = static_cast<float>(height - 1);
            } else {
                posX[i] = posX[i - 1] + stepX;
                posY[i] = posY[i - 1] + stepY;
            }
        }
    }
    onionDrawRings(g, posX.data(), posY.data(), option);
    onionPlaceDoors(g, posX.data(), posY.data(), option, flags);
    (void)mode;
}

}

Grid maze(int width, int height, int option) {
    Grid g = makeGrid(width, height);
    for (int x = 0; x < width; ++x) { g[x][0] = '#'; g[x][height - 1] = '#'; }
    for (int y = 0; y < height; ++y) { g[0][y] = '#'; g[width - 1][y] = '#'; }
    MazeState s;
    s.grid = &g;
    s.width = width;
    s.height = height;
    s.count = ((height - 8) + width) * 2;
    if (s.count < 0) s.count = 0;
    s.xs.assign(s.count, 0);
    s.ys.assign(s.count, 0);
    int n = 0;
    for (int x = 2; x < width - 2 && n < s.count; ++x) {
        s.xs[n] = x; s.ys[n] = 0; ++n;
        if (n < s.count) { s.xs[n] = x; s.ys[n] = height - 1; ++n; }
    }
    for (int y = 2; y < height - 2 && n < s.count; ++y) {
        s.xs[n] = 0; s.ys[n] = y; ++n;
        if (n < s.count) { s.xs[n] = width - 1; s.ys[n] = y; ++n; }
    }
    while (s.count > 0) {
        int x, y;
        mazePick(s, x, y);
        if (option == 0) mazeCarveB(s, x, y);
        else mazeCarveA(s, x, y);
    }
    return g;
}

Grid onion(int width, int height, unsigned int flags, int option) {
    Grid g = makeGrid(width, height);
    if (flags == 0) {
        int r = rnd() % 3;
        if (r == 0) flags = 1;
        else if (r == 1) flags = 4;
        else flags = 8;
        if (rnd() & 1) flags |= 2;
        if (rnd() & 1) flags |= 0x10;
    }
    if ((flags & 0x20) == 0) {
        for (int x = 0; x < width; ++x) { g[x][0] = '#'; g[x][height - 1] = '#'; }
        for (int y = 0; y < height; ++y) { g[0][y] = '#'; g[width - 1][y] = '#'; }
    }
    if ((flags & 0x40) == 0) {
        if (flags & 8) onionVariant(g, width, height, flags, option, 2);
        else if (flags & 4) onionVariant(g, width, height, flags, option, 1);
        else if (flags & 1) onionVariant(g, width, height, flags, option, 0);
    }
    return g;
}

Grid spiral(int width, int height, unsigned int flags) {
    Grid g = makeGrid(width, height);
    for (int x = 0; x < width; ++x)
        for (int y = 0; y < height; ++y) g[x][y] = '#';
    float limitX = static_cast<float>(width / 2 - 2);
    float limitY = static_cast<float>(height / 2 - 2);
    if (flags == 0) flags = static_cast<unsigned int>(rnd());
    if ((flags & 1) && (flags & 4)) {
        if ((rnd() & 1) == 0 || (flags & 1) == 0) flags -= 4;
        else flags -= 1;
    }
    float scale = 0.4545449912548065f;
    if ((flags & 2) == 0) {
        scale = static_cast<float>(0.45454545454545453 + static_cast<double>(rnd() % 5) / 10.0);
    }
    float sx = scale;
    float sy = scale;
    if ((flags & 4) && width != height) {
        if (height < width) sx = static_cast<float>(static_cast<double>(scale) * static_cast<double>(width) / static_cast<double>(height));
        else sy = static_cast<float>(static_cast<double>(scale) * static_cast<double>(height) / static_cast<double>(width));
    }
    if (flags & 1) {
        float m = (sx < sy) ? sx : sy;
        sx = sy = m;
    }
    float angle = 0.0f;
    float ox = 0.0f;
    float oy = 0.0f;
    for (;;) {
        float absX = static_cast<float>(std::abs(static_cast<int>(ox)));
        if (!(absX < limitX)) break;
        float absY = static_cast<float>(std::abs(static_cast<int>(oy)));
        if (!(absY < limitY)) break;
        double a = static_cast<double>(angle);
        ox = static_cast<float>(static_cast<double>(sx) * a * std::cos(a));
        oy = static_cast<float>(static_cast<double>(sy) * a * std::sin(a));
        int px = static_cast<int>(static_cast<double>(width / 2) + ox);
        int py = static_cast<int>(static_cast<double>(height / 2) + oy);
        if (px >= 0 && px < width && py >= 0 && py < height) g[px][py] = 0;
        angle = static_cast<float>(static_cast<double>(angle) + 0.01);
    }
    int px = static_cast<int>(static_cast<double>(width / 2) + static_cast<double>(ox) + 0.5);
    int py = static_cast<int>(static_cast<double>(height / 2) + static_cast<double>(oy) + 0.5);
    if (px >= 0 && px < width && py >= 0 && py < height) g[px][py] = '<';
    return g;
}

Grid rogue(int width, int height, int option) {
    Grid g = makeGrid(width, height);
    for (int x = 0; x < width; ++x)
        for (int y = 0; y < height; ++y) g[x][y] = '#';
    if (width < 11 || height < 11) {
        int x, y;
        for (x = 1; x < width - 1; ++x)
            for (y = 1; y < height - 1; ++y) g[x][y] = 0;
        x = width - 1;
        y = height - 1;
        g[x / 2][y / 2] = '>';
        g[x / 2][y / 2 + 1] = '<';
        return g;
    }
    int target = rnd() % 10 + 6;
    std::vector<std::array<int, 9>> rooms(static_cast<size_t>(target) + 1);
    for (auto& r : rooms) r.fill(0);
    int attempts = 0;
    int placed = 0;
    double root = std::sqrt(static_cast<double>(target));
    int spanX = static_cast<int>(static_cast<double>(width) / root);
    int spanY = static_cast<int>(static_cast<double>(height) / root);
    while (attempts < 450 && placed < target) {
        int x = rnd() % width;
        int y = rnd() % height;
        int rw = rnd() % spanX + rnd() % spanX + rnd() % spanX;
        int rh = static_cast<int>(0.5 * static_cast<double>(rnd() % spanY + rnd() % spanY + rnd() % spanY));
        int left = x - rw / 2;
        int right = rw % 2 + rw / 2 + x;
        int top = y - rh / 2;
        int bottom = rh % 2 + rh / 2 + y;
        bool ok = true;
        if (right > width - 1 || left < 1 || bottom > height - 1 || top < 1 || rw < 3 || rh < 3) ok = false;
        int slot = 0;
        if (ok) {
            while (slot <= target && rooms[slot][0] != 0) {
                int dx = x - rooms[slot][0]; if (dx < 0) dx = -dx;
                int dy = y - rooms[slot][1]; if (dy < 0) dy = -dy;
                if (dx < (rw + rooms[slot][2]) / 2 + 2 && dy < (rh + rooms[slot][3]) / 2 + 2) ok = false;
                ++slot;
            }
        }
        if (!ok || slot > target) { attempts++; continue; }
        int* room = rooms[slot].data();
        room[0] = x; room[1] = y; room[2] = rw; room[3] = rh;
        room[4] = left; room[5] = top; room[6] = right; room[7] = bottom;
        placed++;
    }
    int active = 0;
    while (active < static_cast<int>(rooms.size()) && rooms[active][0] != 0) ++active;
    if (active == 0) {
        int x, y;
        for (x = 1; x < width - 1; ++x)
            for (y = 1; y < height - 1; ++y) g[x][y] = 0;
        x = width - 1;
        y = height - 1;
        g[x / 2][y / 2] = '>';
        g[x / 2][y / 2 + 1] = '<';
        return g;
    }
    auto roomMask = [&](int x, int y) {
        int m = 0;
        if (x > 0 && g[x - 1][y] != 0 && g[x - 1][y] != '.') m |= 1;
        if (x < width - 1 && g[x + 1][y] != 0 && g[x + 1][y] != '.') m |= 2;
        if (y > 0 && g[x][y - 1] != 0 && g[x][y - 1] != '.') m |= 4;
        if (y < height - 1 && g[x][y + 1] != 0 && g[x][y + 1] != '.') m |= 8;
        return m;
    };
    for (int i = 0; i < active; ++i) {
        bool circle;
        int radius = 0;
        if (option == 1) circle = false;
        else if (option == 2) circle = true;
        else {
            circle = (rnd() % 3 == 0);
            radius = std::min(rooms[i][2], rooms[i][3]) / 2;
        }
        for (int x = rooms[i][4]; x < rooms[i][6]; ++x) {
            for (int y = rooms[i][5]; y < rooms[i][7]; ++y) {
                if (!circle || static_cast<int>(0.5 + std::hypot(static_cast<double>(rooms[i][0] - x), static_cast<double>(rooms[i][1] - y))) <= radius) {
                    g[x][y] = '.';
                }
            }
        }
    }
    for (int i = 0; i + 1 < active; ++i) {
        int ax = rooms[i][0], ay = rooms[i][1];
        int bx = rooms[i + 1][0], by = rooms[i + 1][1];
        bool border = false;
        if ((rnd() & 1) == 0) {
            int x1 = ax, y1 = ay, x2 = bx, y2 = by;
            if (y1 > y2) { std::swap(y1, y2); std::swap(x1, x2); }
            for (int y = y1; y < y2; ++y) {
                if (border || g[x1][y] != '#') {
                    if (border && g[x1][y] == '.') { border = false; g[x1][y - 1] = 'D'; }
                    else if (g[x1][y] != 'D' && g[x1][y] != '.') g[x1][y] = 0;
                } else { border = true; g[x1][y] = 'D'; }
            }
            int x = std::min(x1, x2);
            int y = y2;
            if (g[x][y] == '.') border = false;
            if (g[x][y] == 0 || g[x][y] == '#') border = true;
            for (; x < std::max(x1, x2); ++x) {
                if (border || g[x][y] != '#') {
                    if (border && g[x][y] == '.') { border = false; g[x - 1][y] = 'D'; }
                    else if (g[x][y] != 'D' && g[x][y] != '.') g[x][y] = 0;
                } else { border = true; g[x][y] = 'D'; }
            }
        } else {
            int x1 = ax, y1 = ay, x2 = bx, y2 = by;
            if (x1 > x2) { std::swap(x1, x2); std::swap(y1, y2); }
            for (int x = x1; x < x2; ++x) {
                if (border || g[x][y1] != '#') {
                    if (border && g[x][y1] == '.') { border = false; g[x - 1][y1] = 'D'; }
                    else if (g[x][y1] != 'D' && g[x][y1] != '.') g[x][y1] = 0;
                } else { border = true; g[x][y1] = 'D'; }
            }
            int x = x2;
            int y = std::min(y1, y2);
            if (g[x][y] == '.') border = false;
            if (g[x][y] == 0 || g[x][y] == '#') border = true;
            for (; y < std::max(y1, y2); ++y) {
                if (border || g[x][y] != '#') {
                    if (border && g[x][y] == '.') { border = false; g[x][y - 1] = 'D'; }
                    else if (g[x][y] != 'D' && g[x][y] != '.') g[x][y] = 0;
                } else { border = true; g[x][y] = 'D'; }
            }
        }
    }
    g[rooms[0][0]][rooms[0][1]] = '<';
    g[rooms[active - 1][0]][rooms[active - 1][1]] = '>';
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            if (g[x][y] == '.') g[x][y] = 0;
            if (g[x][y] == 'D') {
                int m = roomMask(x, y);
                if (m != 3 && m != 12) { g[x][y] = 0; x = 0; y = 0; }
            }
        }
    }
    return g;
}

Grid snake(int width, int height) {
    Grid g = makeGrid(width, height);
    for (int x = 0; x < width; ++x) { g[x][0] = '#'; g[x][height - 1] = '#'; }
    for (int y = 0; y < height; ++y) { g[0][y] = '#'; g[width - 1][y] = '#'; }
    if ((rnd() & 1) == 0) {
        bool even = true;
        int step = height / (rnd() % ((height - 5) / 3) + 2);
        for (int y = step; y < height - 3; y += step) {
            if (even) {
                int x;
                for (x = 1; x < width - 2; ++x) g[x][y] = '#';
                g[x][y] = 'D';
            } else {
                int x;
                for (x = 2; x < width; ++x) g[x][y] = '#';
                g[1][y] = 'D';
            }
            even = !even;
        }
    } else {
        bool even = true;
        int step = width / (rnd() % ((width - 5) / 3) + 2);
        for (int x = step; x < width - 3; x += step) {
            if (even) {
                int y;
                for (y = 1; y < height - 2; ++y) g[x][y] = '#';
                g[x][y] = 'D';
            } else {
                int y;
                for (y = 2; y < height; ++y) g[x][y] = '#';
                g[x][1] = 'D';
            }
            even = !even;
        }
    }
    if ((rnd() & 1) == 0) {
        g[1][1] = '>';
        g[width - 2][height - 2] = '<';
    } else {
        g[1][1] = '<';
        g[width - 2][height - 2] = '>';
    }
    return g;
}

Grid squareSpiral(int width, int height) {
    Grid g = onion(width, height, 0x101, 0);
    int cx = 0, cy = 0;
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            if (g[x][y] == 'C') { cx = x; cy = y; }
        }
    }
    int x = cx, y = cy;
    for (;;) {
        y--;
        while (y >= 0 && g[x][y] == 0) y--;
        while (y + 1 < height && g[x][y + 1] == 0) x++;
        if (y < 2 || x < 2 || x > width - 2 || y > height - 2) break;
        carveDoor(g, x, y - 1, 1);
        g[x][y - 1] = '#';
        g[x - 1][y] = 'D';
        while (g[x - 1][y] != 0) x--;
        carveDoor(g, x - 1, y, 0);
        while (g[x][y + 1] != 0) y++;
        carveDoor(g, x, y + 1, 1);
        while (g[x + 1][y] != 0) x++;
        carveDoor(g, x + 1, y, 0);
        x++;
    }
    if ((rnd() & 1) == 0) {
        g[cx][cy] = '<';
        g[width - 2][1] = '>';
    } else {
        g[cx][cy] = '>';
        g[width - 2][1] = '<';
    }
    return g;
}

void symmetryFix(Grid& g, int width, int height, int symmetry) {
    int cx = width / 2;
    int cy = height / 2;
    if (symmetry == 2) {
        g[cx][cy] = 0;
        int x = cx;
        while (--x > 0 && g[x][cy] == '#') g[x][cy] = 0;
        int prev = cx;
        while ((x = prev + 1) < width - 1 && g[x][cy] == '#') { g[x][cy] = 0; prev = x; }
    }
    if (symmetry == 3) {
        g[cx][cy] = 0;
        int y = cy;
        while (--y > 0 && g[cx][y] == '#') g[cx][y] = 0;
        int prev = cy;
        while ((y = prev + 1) < height - 1 && g[cx][y] == '#') { g[cx][y] = 0; prev = y; }
    }
    if (symmetry == 4) {
        g[cx][cy / 2] = 0;
        g[cx / 2][cy] = 0;
        g[cx][cy / 2 + cy] = 0;
        g[cx / 2 + cx][cy] = 0;
        int x = cx;
        while (--x > 0 && g[x][cy / 2] == '#') { g[x][cy / 2 + cy] = 0; g[x][cy / 2] = 0; }
        x = cx;
        while (++x < width - 1 && g[x][cy / 2] == '#') { g[x][cy / 2 + cy] = 0; g[x][cy / 2] = 0; }
        int y = cy;
        while (--y > 0 && g[cx / 2][y] == '#') { g[cx / 2][y] = 0; g[cx / 2 + cx][y] = 0; }
        y = cy;
        while (++y < height - 1 && g[cx / 2][y] == '#') { g[cx / 2][y] = 0; g[cx / 2 + cx][y] = 0; }
    }
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            if (g[x][y] == 'D') {
                int m = 0;
                if (x > 0 && g[x - 1][y] != 0 && g[x - 1][y] != '.') m |= 1;
                if (x < width - 1 && g[x + 1][y] != 0 && g[x + 1][y] != '.') m |= 2;
                if (y > 0 && g[x][y - 1] != 0 && g[x][y - 1] != '.') m |= 4;
                if (y < height - 1 && g[x][y + 1] != 0 && g[x][y + 1] != '.') m |= 8;
                if (m != 3 && m != 12) { g[x][y] = 0; x = 0; y = 0; }
            }
        }
    }
}

void applySymmetry(Grid& grid, int symmetry, int& width, int& height, int styleNumber) {
    if (symmetry == 1) return;
    int oldW = width;
    int oldH = height;
    int newW = (symmetry == 2 || symmetry == 4) ? oldW * 2 - 3 : oldW;
    int newH = (symmetry == 3 || symmetry == 4) ? oldH * 2 - 3 : oldH;
    Grid out = makeGrid(newW, newH);
    if (symmetry == 2) {
        for (int x = 0; x < newW / 2 + 1; ++x) {
            for (int y = 0; y < newH; ++y) {
                out[x][y] = grid[x][y];
                out[newW - 1 - x][y] = grid[x][y];
            }
        }
    }
    if (symmetry == 3) {
        for (int x = 0; x < newW; ++x) {
            for (int y = 0; y < newH / 2 + 1; ++y) {
                out[x][y] = grid[x][y];
                out[x][newH - 1 - y] = grid[x][y];
            }
        }
    }
    if (symmetry == 4) {
        for (int x = 0; x < newW / 2 + 1; ++x) {
            for (int y = 0; y < newH / 2 + 1; ++y) {
                out[x][y] = grid[x][y];
                out[x][newH - 1 - y] = grid[x][y];
                out[newW - 1 - x][y] = grid[x][y];
                out[newW - 1 - x][newH - 1 - y] = grid[x][y];
            }
        }
    }
    grid = std::move(out);
    width = newW;
    height = newH;
    if (styleNumber == 3 || styleNumber == 4) symmetryFix(grid, width, height, symmetry);
}

Grid rotate(const Grid& grid, int mode, int& width, int& height) {
    if (mode == 0) return grid;
    if (mode == 1 || mode == 3) {
        Grid out(height, std::vector<char>(width, 0));
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                out[y][x] = (mode == 1) ? grid[x][y] : grid[width - 1 - x][height - 1 - y];
            }
        }
        std::swap(width, height);
        return out;
    }
    Grid out(width, std::vector<char>(height, 0));
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            out[x][y] = grid[width - 1 - x][height - 1 - y];
        }
    }
    return out;
}

Grid doubleScale(const Grid& grid, int width, int height) {
    Grid out = makeGrid(width * 2 - 1, height * 2 - 1);
    auto bits = [&](char c, int x, int y) {
        int m = 0;
        if (x + 1 < width && grid[x + 1][y] == c) m |= 1;
        if (y + 1 < height) {
            if (grid[x][y + 1] == c) m |= 2;
            if (x + 1 < width && grid[x + 1][y + 1] == c) m |= 4;
        }
        return m;
    };
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            char c = grid[x][y];
            if (c == '#') {
                int m1 = bits('#', x, y);
                int m2 = bits('D', x, y);
                out[x * 2][y * 2] = '#';
                if (x + 1 < width && ((m1 | m2) & 1)) out[x * 2 + 1][y * 2] = grid[x + 1][y];
                if (y + 1 < height) {
                    if ((m1 | m2) & 2) out[x * 2][y * 2 + 1] = grid[x][y + 1];
                    if (m1 == 7) out[x * 2 + 1][y * 2 + 1] = '#';
                }
            } else if (c == 'D') {
                int m1 = bits('#', x, y);
                int m2 = bits('D', x, y);
                if ((m1 & 3) != 0) m2 = m1;
                out[x * 2][y * 2] = 'D';
                if (x + 1 < width && (m2 & 1)) out[x * 2 + 1][y * 2] = 'D';
                if (y + 1 < height && (m2 & 2)) out[x * 2][y * 2 + 1] = 'D';
            } else {
                out[x * 2][y * 2] = c;
            }
        }
    }
    return out;
}

void orphanWallCleanup(Grid& g, int width, int height) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (x > 0 && x <= width - 3 && y > 2 && g[x][y] != 0 && g[x + 2][y - 2] != 0 &&
                g[x + 1][y] == 0 && g[x - 1][y - 1] == 0 && g[x][y - 1] == 0) {
                g[x][y] = ' ';
            }
            if (x <= width - 3 && y <= height - 3 && y > 1 && g[x][y] != 0 &&
                g[x + 1][y] == 0 && g[x][y + 1] == 0 && g[x + 1][y + 1] == 0 &&
                g[x][y - 1] != 0 && g[x - 1][y] != 0 && g[x + 2][y + 2] != 0) {
                g[x][y] = ' ';
            }
        }
    }
}

void onionFinishing(Grid& g, int width, int height) {
    int iterations = (width * height) / 30;
    for (int i = 0; i < iterations; ++i) {
        int x = rnd() % width;
        int y = rnd() % height;
        int d0 = corridorOpen(g, x, y, 0, width, height);
        int d1 = corridorOpen(g, x, y, 1, width, height);
        if (d0 == -1) {
            if (d1 != -1) carveDoor(g, x, y, 1);
        } else if (d1 == -1) {
            carveDoor(g, x, y, 0);
        } else if (d0 < d1) {
            carveDoor(g, x, y, 0);
        } else {
            carveDoor(g, x, y, 1);
        }
    }
}

}
