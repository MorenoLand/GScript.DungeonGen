#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

namespace Utils {

std::string trim(const std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);
char getBase64Char(int val);
int parseTileVal(const std::string& str);
int randomInt(int maxVal);

} // namespace Utils

#endif // UTILS_H
