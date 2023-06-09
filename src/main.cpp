#include "debug.h"
#include "config.h"

#include "LexicalAnalysis/lexical_analysis.h"
#include "Parser/parser.h"
#include "Parser/syntax_tree.h"
#include "CodeGenerator/code_generator.h"
#include "Error/error.h"
#include "LexicalAnalysis/token.h"
#include "RunTime/run_time.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

int main() {
    #if DEBUG
        std::cout << "Process start" << std::endl;
    #endif
    std::vector<Token*> tokens;
    SyntaxTree* root = NULL;

    // LexicalAnalysis
    #if ENABLE_LEXICAL_ANALYSIS
    try {
        #if DEBUG
            std::cout << "LexicalAnalysis start" << std::endl;
        #endif
        LexicalAnalysis* la = new LexicalAnalysis(INPUT_SOURCE_FILE);
        tokens = la->run();
        delete la;
        #if DEBUG
            std::cout << "LexicalAnalysis end" << std::endl;
        #endif
    }
    catch (...) {
        error("Exception: LexicalAnalysis");
    }
    #endif

    // Parser
    #if ENABLE_PARSER
    try {
        #if DEBUG
            std::cout << "Parser start" << std::endl;
        #endif
        Parser* parser = new Parser(tokens);
        parser->run();
        root = parser->getTree();
        delete parser;
        #if DEBUG
            std::cout << "Parser end" << std::endl;
        #endif
    }
    catch (...) {
        error("Exception: Parser");
    }
    #endif

    // CodeGeneration
    #if ENABLE_CODE_GENERATION
    try {
        #if DEBUG
            std::cout << "CodeGeneration start" << std::endl;
        #endif
        CodeGenerator* cg = new CodeGenerator(root, OUTPUT_DUMP_FILE, OUTPUT_RUNTIME_FILE);
        cg->run();
        delete cg;
        #if DEBUG
            std::cout << "CodeGeneration end" << std::endl;
        #endif
    }
    catch (...) {
        error("Exception: CodeGeneration");
    }
    #endif

    // RunTime
    #if ENABLE_RUNTIME
    try {
        #if DEBUG
            std::cout << "RunTime start" << std::endl;
        #endif
        RunTime* rt = new RunTime(OUTPUT_RUNTIME_FILE);
        clock_t start = clock();
        rt->run();
        clock_t end = clock();
        std::cout << "RunTime: " << (double)(end - start) / CLOCKS_PER_SEC / 10 << "s" << std::endl;
        delete rt;
        #if DEBUG
            std::cout << "RunTime end" << std::endl;
        #endif
    }
    catch (...) {
        error("Exception: RunTime");
    }
    #endif

    for (Token* t : tokens) {
        delete t;
    }
    tokens.clear();
    if (root != NULL) {
        delete root;
    }

    #if DEBUG
        std::cout << "Process end" << std::endl;
    #endif

    return 0;
}
