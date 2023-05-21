#ifndef _SYNTAX_TREE_NODE_
#define _SYNTAX_TREE_NODE_

#include "../LexicalAnalysis/token.h"
#include "ast_enum.h"

#include <vector>
#include <iostream>

class SyntaxTreeNode {
    protected:
        Token* token;
        SyntaxTreeNode* parent;
        std::vector<SyntaxTreeNode*> children;
        ASTNodeType type;
    public:
        SyntaxTreeNode(Token* token, ASTNodeType type);
        ~SyntaxTreeNode();
        void addChild(SyntaxTreeNode* child);
        Token* getToken();
        SyntaxTreeNode* getParent();
        std::vector<SyntaxTreeNode*> getChildren();
        ASTNodeType getType();
        void setParent(SyntaxTreeNode* parent);
        void printTree(int depth);
        std::string __str(void);
};

#endif