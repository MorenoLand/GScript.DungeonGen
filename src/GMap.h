#ifndef GMAP_H
#define GMAP_H

#include <string>

class GMap {
public:
    int width = 0;
    int height = 0;
    std::string generatedLastLevel;

    bool saveToFile(const std::string& filename) const;
};

#endif // GMAP_H
