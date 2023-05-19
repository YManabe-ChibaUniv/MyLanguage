#include "syntax_tree_node.h"

SyntaxTreeNode::SyntaxTreeNode(Token* token, ASTNodeType type) {
    this->token = token;
    this->parent = NULL;
    this->type = type;
}

SyntaxTreeNode::~SyntaxTreeNode() {
    for (auto child : this->children) {
        delete child;
    }
}

void SyntaxTreeNode::addChild(SyntaxTreeNode* child) {
    this->children.push_back(child);
    child->setParent(this);
}

Token* SyntaxTreeNode::getToken() {
    return this->token;
}

SyntaxTreeNode* SyntaxTreeNode::getParent() {
    return this->parent;
}

std::vector<SyntaxTreeNode*> SyntaxTreeNode::getChildren() {
    return this->children;
}

ASTNodeType SyntaxTreeNode::getType() {
    return this->type;
}

void SyntaxTreeNode::setParent(SyntaxTreeNode* parent) {
    this->parent = parent;
}

void SyntaxTreeNode::printTree(int depth) {
    for (int i = 0; i < depth; i++) {
        std::cout << "    |";
    }

    if (depth > 0) {
        std::cout << "-- ";
    }

    switch (this->type) {
        case ASTNodeType::PROGRAM:
            std::cout << "PROGRAM";
            break;
        case ASTNodeType::FUNCTION_DEFINITION:
            std::cout << "FUNCTION_DEFINITION";
            break;
        case ASTNodeType::FUNCTION_NAME:
            std::cout << "FUNCTION_NAME";
            break;
        case ASTNodeType::PARAM_LIST:
            std::cout << "PARAM_LIST";
            break;
        case ASTNodeType::BLOCK:
            std::cout << "BLOCK";
            break;
        case ASTNodeType::VAR_DEFINITION:
            std::cout << "VAR_DEFINITION";
            break;
        case ASTNodeType::VAR_TYPE:
            std::cout << "VAR_TYPE";
            break;
        case ASTNodeType::VAR_NAME:
            std::cout << "VAR_NAME";
            break;
        case ASTNodeType::ASSIGNMENT_EXPRESSION:
            std::cout << "ASSIGNMENT_EXPRESSION";
            break;
        case ASTNodeType::INT_LITERAL:
            std::cout << "INT_LITERAL";
            break;
        case ASTNodeType::STRING_LITERAL:
            std::cout << "STRING_LITERAL";
            break;
        case ASTNodeType::FUNCTION_CALL:
            std::cout << "FUNCTION_CALL";
            break;
        case ASTNodeType::ARGUMENT_LIST:
            std::cout << "ARGUMENT_LIST";
            break;
        case ASTNodeType::BINARY_EXPRESSION:
            std::cout << "BINARY_EXPRESSION";
            break;
        case ASTNodeType::OPERATOR:
            std::cout << "OPERATOR";
            break;
        case ASTNodeType::RETURN_STATEMENT:
            std::cout << "RETURN_STATEMENT";
            break;
        case ASTNodeType::RETURN_LIST:
            std::cout << "RETURN_LIST";
            break;
        case ASTNodeType::RETURN_TYPE:
            std::cout << "RETURN_TYPE";
            break;
        default:
            std::cout << "UNKNOWN";
            break;
    }

    if (this->token != NULL) {
        std::cout << " : " << this->token->getValue() << std::endl;
    }
    else {
        std::cout << std::endl;
    }

    for (SyntaxTreeNode* child : this->children) {
        child->printTree(depth + 1);
    }

    return;
}
