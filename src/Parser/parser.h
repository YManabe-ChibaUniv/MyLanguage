#ifndef _PARSER_
#define _PARSER_

#define SYNTAX_TREE_FILE "dump/syntax_tree.txt"

#include "syntax_tree_node.h"
#include "syntax_tree.h"
#include "ast_enum.h"
#include "../LexicalAnalysis/token.h"
#include "../LexicalAnalysis/token_enum.h"
#include "../LexicalAnalysis/token_type.h"
#include "../debug.h"

#include <vector>
#include <iostream>
#include <experimental/source_location>
#include <fstream>
#include <stack>

class Parser {
    protected:
        std::vector<Token*> tokens;
        int index;
        SyntaxTree* tree;
        SyntaxTreeNode* parseProgram(void);
        std::vector<SyntaxTreeNode*> parseStatement(void);
        SyntaxTreeNode* parseExpression(void);
        SyntaxTreeNode* parseElement(void);
        SyntaxTreeNode* parseTerm(void);
        SyntaxTreeNode* parseFactor(void);
        SyntaxTreeNode* parseOperator(void);
        SyntaxTreeNode* parseReturnStatement(void);
        SyntaxTreeNode* parseFunctionCall(void);
        SyntaxTreeNode* parseFunctionDefinition(void);
        std::vector<SyntaxTreeNode*> parseVariableDefinition(void);
        SyntaxTreeNode* parseVariableAssignment(void);
        SyntaxTreeNode* parseBlockStatement(void);
        SyntaxTreeNode* parseBinaryExpression(void);
        SyntaxTreeNode* parseArgumentList(void);
        SyntaxTreeNode* parseIfStatement(void);
        SyntaxTreeNode* parseIfExpression(void);
        SyntaxTreeNode* parseIfTerm(void);
        SyntaxTreeNode* parseIfFactor(void);
        SyntaxTreeNode* parseIfOperator(void);
        SyntaxTreeNode* parseWhileStatement(void);
        void runError(Token* token, const std::experimental::fundamentals_v2::source_location& location = std::experimental::fundamentals_v2::source_location::current());
    public:
        Parser(std::vector<Token*> tokens);
        ~Parser();
        SyntaxTree* getTree(void);
        void printTree(void);
        void run(void);
};

#endif
