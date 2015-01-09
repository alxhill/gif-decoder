#include <iostream>
#include "gif.hpp"

int main(int argc, const char * argv[]) {
    std::cout << "GIF Parser" << std::endl;

    std::string filename;
    if (argc < 2)
        filename = "test.gif";
    else
        filename = std::string(argv[1]);

    GIF gif(filename);

    return 0;
}
