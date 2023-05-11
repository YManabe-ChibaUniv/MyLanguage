#include "error.h"

void error(const char *msg) {
    std::cerr << msg << std::endl;
    exit(1);
}
