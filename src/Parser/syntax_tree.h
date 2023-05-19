#ifndef _SYNTAX_TREE_
#define _SYNTAX_TREE_

#include "../LexicalAnalysis/token.h"
#include "syntax_tree_node.h"

#include <vector>

class SyntaxTree {
    protected:
        SyntaxTreeNode* root;
    public:
        SyntaxTree(SyntaxTreeNode* root);
        ~SyntaxTree();
        SyntaxTreeNode* getRoot();
        void printTree();
};
#endif
