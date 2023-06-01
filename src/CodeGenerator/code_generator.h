#ifndef _CODE_GENERATOR_
#define _CODE_GENERATOR_

#include "op_code.h"
#include "instruction.h"
#include "../Parser/syntax_tree.h"
#include "../LexicalAnalysis/token_enum.h"
#include "var_table.h"
#include "function_table.h"
#include "../config.h"
#include "../debug.h"

#include <vector>
#include <fstream>
#include <cstdint>
#include <stack>
#include <sstream>
#include <iomanip>
#include <map>
#include <cstring>

class CodeGenerator {
    public:
        CodeGenerator(SyntaxTree* root, std::string outputDumpFileName, std::string outputRuntimeFileName);
        ~CodeGenerator();
        void run(void);
    private:
        std::ofstream outputDumpFile;
        std::ofstream outputRuntimeFile;
        std::ofstream outputOpCOdeFile;
        std::vector<Instruction*> instructions;
        SyntaxTree* root;
        VarTable* varTable;
        FunctionTable* functionTable;
        std::streampos mainFunctionAddress;
        std::streampos mainCallAddress;
        std::map<int, std::vector<int>> whileBreakAddressStack;
        std::map<int, std::vector<int>> whileContinueAddressStack;
        void codeGen(SyntaxTreeNode* node, int whileDepth);
        void printInstructions(std::vector<SyntaxTreeNode*> nodes, int whileDepth);
        void storeValue(SyntaxTreeNode* node);
        void writeIntData(int value);
        void writeFloatData(float value);
        void writeOperator(TokenDetail td);
        void writeAddress(int value, int writeAddress);
        void logOpCodes(void);
};

#endif
