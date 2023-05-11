#include "debug.h"

#define INPUT_SOURCE_FILE "source.m"

#include "LexicalAnalysis/lexical_analysis.h"
#include "Error/error.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

int main() {
    std::cout << "Process start" << std::endl;
    std::vector<Token*> tokens;
    try{
        std::cout << "LexicalAnalysis start" << std::endl;
        LexicalAnalysis* la = new LexicalAnalysis(INPUT_SOURCE_FILE);
        tokens = la->run();
        delete(la);
        std::for_each(tokens.begin(), tokens.end(), [](Token* p) {
            delete(p);
        });
        tokens.clear();
        std::cout << "LexicalAnalysis end" << std::endl;
    }
    catch(...) {
        error("Exception: LexicalAnalysis");
    }
    std::cout << "Process end" << std::endl;
    return 0;
}
