#include "DungeonGenerator.h"
#include <string>

int main(int argc, char* argv[]) {
    std::string optionsFilename = "dungeonoptions.txt";
    if (argc > 1) {
        optionsFilename = argv[1];
    }

    DungeonGenerator generator;
    generator.run(optionsFilename);

    return 0;
}
