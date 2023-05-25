#ifndef _CODE_GENERATOR_
#define _CODE_GENERATOR_

#include "op_code.h"
#include "instruction.h"
#include "../Parser/syntax_tree.h"
#include "../LexicalAnalysis/token_enum.h"
#include "var_table.h"
#include "function_table.h"

#include <vector>
#include <fstream>
#include <cstdint>

class CodeGenerator {
    public:
        CodeGenerator(SyntaxTree* root, std::string outputDumpFileName, std::string outputRuntimeFileName);
        ~CodeGenerator();
        void run(void);
    private:
        std::ofstream outputDumpFile;
        std::ofstream outputRuntimeFile;
        std::vector<Instruction*> instructions;
        SyntaxTree* root;
        VarTable* varTable;
        FunctionTable* functionTable;
        std::streampos mainFunctionAddress;
        std::streampos mainCallAddress;
        void codeGen(SyntaxTreeNode* node);
        void printInstructions(std::vector<SyntaxTreeNode*> nodes);
        void storeValue(SyntaxTreeNode* node);
        void writeIntData(int value);
        void writeOperator(TokenDetail td);
};

#endif
