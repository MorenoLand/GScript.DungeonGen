#include "Utils.h"
#include <algorithm>
#include <sstream>
#include <cstdlib>

namespace Utils {

static const char BASE64_TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

char getBase64Char(int val) {
    val &= 0x3F;
    return BASE64_TABLE[val];
}

int parseTileVal(const std::string& str) {
    std::string s = trim(str);
    if (s.empty()) return 0;
    if (s.rfind("0x", 0) == 0 || s.rfind("0X", 0) == 0) {
        return static_cast<int>(std::strtoul(s.c_str(), nullptr, 16));
    }
    return std::atoi(s.c_str());
}

int randomInt(int maxVal) {
    if (maxVal <= 0) return 0;
    return std::rand() % maxVal;
}

} // namespace Utils
