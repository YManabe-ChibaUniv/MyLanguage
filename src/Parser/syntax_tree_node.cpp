#include "syntax_tree_node.h"

SyntaxTreeNode::SyntaxTreeNode(Token* token, ASTNodeType type) {
    this->token = token;
    this->parent = NULL;
    this->type = type;
}

SyntaxTreeNode::~SyntaxTreeNode() {
    for (SyntaxTreeNode* child : this->getChildren()) {
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
        if (i == 0) {
            std::cout << " |";
        }
        else {
            std::cout << "    |";
        }
    }

    if (depth > 0) {
        std::cout << "-- ";
    }

    std::cout << this->__str();

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

std::string SyntaxTreeNode::__str(void) {
    switch (this->type) {
        case ASTNodeType::PROGRAM:
            return "PROGRAM";
            break;
        case ASTNodeType::FUNCTION_DEFINITION:
            return "FUNCTION_DEFINITION";
            break;
        case ASTNodeType::FUNCTION_NAME:
            return "FUNCTION_NAME";
            break;
        case ASTNodeType::PARAM_LIST:
            return "PARAM_LIST";
            break;
        case ASTNodeType::BLOCK:
            return "BLOCK";
            break;
        case ASTNodeType::VAR_DEFINITION:
            return "VAR_DEFINITION";
            break;
        case ASTNodeType::VAR_TYPE:
            return "VAR_TYPE";
            break;
        case ASTNodeType::VAR_NAME:
            return "VAR_NAME";
            break;
        case ASTNodeType::ASSIGNMENT_EXPRESSION:
            return "ASSIGNMENT_EXPRESSION";
            break;
        case ASTNodeType::INT_LITERAL:
            return "INT_LITERAL";
            break;
        case ASTNodeType::FLOAT_LITERAL:
            return "FLOAT_LITERAL";
            break;
        case ASTNodeType::STRING_LITERAL:
            return "STRING_LITERAL";
            break;
        case ASTNodeType::BOOLEAN_LITERAL:
            return "BOOLEAN_LITERAL";
            break;
        case ASTNodeType::FUNCTION_CALL:
            return "FUNCTION_CALL";
            break;
        case ASTNodeType::ARGUMENT_LIST:
            return "ARGUMENT_LIST";
            break;
        case ASTNodeType::EXPRESSION:
            return "EXPRESSION";
            break;
        case ASTNodeType::OPERATOR:
            return "OPERATOR";
            break;
        case ASTNodeType::RETURN_STATEMENT:
            return "RETURN_STATEMENT";
            break;
        case ASTNodeType::RETURN_LIST:
            return "RETURN_LIST";
            break;
        case ASTNodeType::RETURN_TYPE:
            return "RETURN_TYPE";
            break;
        case ASTNodeType::IF_STATEMENT:
            return "IF_STATEMENT";
            break;
        case ASTNodeType::WHILE_STATEMENT:
            return "WHILE_STATEMENT";
            break;
        case ASTNodeType::BREAK:
            return "BREAK";
            break;
        case ASTNodeType::CONTINUE:
            return "CONTINUE";
            break;
    }
    return "UNKNOWN";
}

SyntaxTreeNode* SyntaxTreeNode::clone(void) {
    SyntaxTreeNode* clone = new SyntaxTreeNode(this->token, this->type);
    for (SyntaxTreeNode* child : this->children) {
        clone->addChild(child->clone());
    }
    return clone;
}
