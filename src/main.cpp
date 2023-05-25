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

int main() {
    #if DEBUG
        std::cout << "Process start" << std::endl;
    #endif
    std::vector<Token*> tokens;
    SyntaxTree* root;

    // LexicalAnalysis
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

    // Parser
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

    // CodeGeneration
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

    // RunTime
    try {
        #if DEBUG
            std::cout << "RunTime start" << std::endl;
        #endif
        RunTime* rt = new RunTime(OUTPUT_RUNTIME_FILE);
        rt->run();
        delete rt;
        #if DEBUG
            std::cout << "RunTime end" << std::endl;
        #endif
    }
    catch (...) {
        error("Exception: RunTime");
    }

    for (Token* t : tokens) {
        delete t;
    }
    tokens.clear();
    delete root;

    #if DEBUG
        std::cout << "Process end" << std::endl;
    #endif

    return 0;
}
