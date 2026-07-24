#include "GMap.h"
#include <fstream>
#include <iostream>

bool GMap::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open gmap file for writing: " << filename << std::endl;
        return false;
    }

    file << "GRMAP001\n"
         << "WIDTH " << width << "\n"
         << "HEIGHT " << height << "\n"
         << "GENERATED " << generatedLastLevel << "\n";

    return true;
}
