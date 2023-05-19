#include "syntax_tree.h"

SyntaxTree::SyntaxTree(SyntaxTreeNode* root) {
    this->root = root;
}

SyntaxTree::~SyntaxTree() {
    delete this->root;
}

SyntaxTreeNode* SyntaxTree::getRoot() {
    return this->root;
}

void SyntaxTree::printTree() {
    this->root->printTree(0);
}
