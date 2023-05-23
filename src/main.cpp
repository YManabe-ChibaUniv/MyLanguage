#include "debug.h"

#define INPUT_SOURCE_FILE "source.m"
#define OUTPUT_DUMP_FILE "dump/code.txt"
#define OUTPUT_RUNTIME_FILE "dump/runtime"

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
    std::cout << "Process start" << std::endl;
    std::vector<Token*> tokens;
    SyntaxTree* root;

    // LexicalAnalysis
    try {
        std::cout << "LexicalAnalysis start" << std::endl;
        LexicalAnalysis* la = new LexicalAnalysis(INPUT_SOURCE_FILE);
        tokens = la->run();
        delete la;
        std::cout << "LexicalAnalysis end" << std::endl;
    }
    catch (...) {
        error("Exception: LexicalAnalysis");
    }

    // Parser
    try {
        std::cout << "Parser start" << std::endl;
        Parser* parser = new Parser(tokens);
        parser->run();
        root = parser->getTree();
        delete parser;
        std::cout << "Parser end" << std::endl;
    }
    catch (...) {
        error("Exception: Parser");
    }

    // CodeGeneration
    try {
        std::cout << "CodeGeneration start" << std::endl;
        CodeGenerator* cg = new CodeGenerator(root, OUTPUT_DUMP_FILE, OUTPUT_RUNTIME_FILE);
        cg->run();
        delete cg;
        std::cout << "CodeGeneration end" << std::endl;
    }
    catch (...) {
        error("Exception: CodeGeneration");
    }

    // RunTime
    try {
        std::cout << "RunTime start" << std::endl;
        RunTime* rt = new RunTime(OUTPUT_RUNTIME_FILE);
        rt->run();
        delete rt;
        std::cout << "RunTime end" << std::endl;
    }
    catch (...) {
        error("Exception: RunTime");
    }

    for (Token* t : tokens) {
        delete t;
    }
    tokens.clear();
    delete root;

    std::cout << "Process end" << std::endl;

    return 0;
}
