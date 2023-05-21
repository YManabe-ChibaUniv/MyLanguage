#include "code_generator.h"

CodeGenerator::CodeGenerator(SyntaxTree* root, std::string outputFileName) {
    this->root = root;
    this->outputFile.open(outputFileName);
    this->varTable = new VarTable();
    this->functionTable = new FunctionTable();
}

CodeGenerator::~CodeGenerator() {
    for (Instruction* i : this->instructions) {
        delete i;
    }
    this->instructions.clear();
    this->outputFile.close();
    delete this->varTable;
    delete this->functionTable;
}

void CodeGenerator::run(void) {
    this->codeGen(this->root->getRoot());
}

void CodeGenerator::codeGen(SyntaxTreeNode* node) {
    SyntaxTreeNode* tmpNode;
    std::vector<SyntaxTreeNode*>::iterator itr;
    std::vector<SyntaxTreeNode*> childNodes;
    TokenDetail td;
    int varAddress;

    switch (node->getType()) {
        case ASTNodeType::PROGRAM:
            for (SyntaxTreeNode* child : node->getChildren()) {
                this->codeGen(child);
            }
            // TODO: FUNCTION_CALL mainのアドレスに飛ばす
            break;
        case ASTNodeType::FUNCTION_DEFINITION:
            childNodes = node->getChildren();
            itr = childNodes.begin();
            // FUNCTION_NAME
            tmpNode = *itr;
            this->outputFile << tmpNode->getToken()->getValue() << ":" << std::endl;
            // 関数ラベルのアドレスを記録
            itr++;
            // PARAMETER_LIST
            // TODO: PROCESS PARAMETER_LIST
            tmpNode = *itr;
            itr++;
            // RETURN_TYPE
            tmpNode = *itr;
            itr++;
            // BLOCK
            tmpNode = *itr;
            for (SyntaxTreeNode* child : tmpNode->getChildren()) {
                this->codeGen(child);
            }
            break;
        case ASTNodeType::VAR_DEFINITION:
            childNodes = node->getChildren();
            itr = childNodes.begin();
            // VAR_TYPE
            tmpNode = *itr;
            td = tmpNode->getToken()->getTokenType()->getTokenDetail();
            itr++;
            // VAR_NAME
            tmpNode = *itr;
            switch (td) {
                case TokenDetail::DEF_INT32:
                    this->varTable->add(tmpNode->getToken()->getValue(), TokenDetail::DEF_INT32);
                    break;
                case TokenDetail::DEF_STRING:
                    this->varTable->add(tmpNode->getToken()->getValue(), TokenDetail::DEF_STRING);
                    break;
                default:
                    break;
            }
            break;
        case ASTNodeType::ASSIGNMENT_EXPRESSION:
            childNodes = node->getChildren();
            itr = childNodes.begin();
            // VAR_NAME
            tmpNode = *itr;
            varAddress = this->varTable->getAddress(tmpNode->getToken()->getValue());
            itr++;
            // INT_LITERAL or STRING_LITERAL or VAR_NAME
            tmpNode = *itr;
            switch (tmpNode->getToken()->getTokenType()->getTokenKind()) {
                // INT_LITERAL
                case TokenKind::TK_NUM:
                    this->outputFile << "PUSH_INT " << tmpNode->getToken()->getValue() << std::endl;
                    this->outputFile << "STORE_INT " << varAddress << std::endl;
                    break;
                // STRING_LITERAL
                case TokenKind::TK_STRING:
                    this->outputFile << "PUSH_STRING " << tmpNode->getToken()->getValue() << std::endl;
                    this->outputFile << "STORE_STRING " << varAddress << std::endl;
                    break;
                // VAR_NAME
                case TokenKind::TK_IDENT:
                    switch (this->varTable->getType(varAddress)) {
                        case TokenDetail::DEF_INT32:
                            this->outputFile << "LOAD_INT " << this->varTable->getAddress(tmpNode->getToken()->getValue()) << std::endl;
                            this->outputFile << "STORE_INT " << varAddress << std::endl;
                            break;
                        case TokenDetail::DEF_STRING:
                            this->outputFile << "LOAD_STRING " << this->varTable->getAddress(tmpNode->getToken()->getValue()) << std::endl;
                            this->outputFile << "STORE_STRING " << varAddress << std::endl;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            break;
        case ASTNodeType::FUNCTION_CALL:
            childNodes = node->getChildren();
            itr = childNodes.begin();
            // FUNCTION_NAME
            tmpNode = *itr;
            itr++;
            // print
            if (tmpNode->getToken()->getValue() == "print") {
                tmpNode = *itr;
                this->printInstructions(tmpNode->getChildren());
            }
            // println
            else if (tmpNode->getToken()->getValue() == "println") {
                tmpNode = *itr;
                this->printInstructions(tmpNode->getChildren());
                this->outputFile << "PUSH_STRING \\n" << std::endl;
                this->outputFile << "PRINT" << std::endl;
            }
            else {
                this->outputFile << "JUMP " << tmpNode->getToken()->getValue() << std::endl;
                if (tmpNode->getToken()->getValue() == "main") {
                    this->outputFile << "HALT" << std::endl;
                }
            }
            break;
        case ASTNodeType::BINARY_EXPRESSION:
            childNodes = node->getChildren();
            itr = childNodes.begin();
            // INT_LITERAL or STRING_LITERAL or VAR_NAME
            tmpNode = *itr;
            this->storeValue(tmpNode);
            itr++;
            // OPERATOR
            tmpNode = *itr;
            td = tmpNode->getToken()->getTokenType()->getTokenDetail();
            itr++;
            // INT_LITERAL or STRING_LITERAL or VAR_NAME
            tmpNode = *itr;
            this->storeValue(tmpNode);
            switch (td) {
                case TokenDetail::PLUS:
                    this->outputFile << "ADD" << std::endl;
                    break;
                case TokenDetail::MINUS:
                    this->outputFile << "SUB" << std::endl;
                    break;
                default:
                    break;
            }
            break;
        case ASTNodeType::RETURN_STATEMENT:
            this->outputFile << "RETURN" << std::endl;
            break;
        default:
            break;
    }
}

void CodeGenerator::printInstructions(std::vector<SyntaxTreeNode*> nodes) {
    for (SyntaxTreeNode* node : nodes) {
        // BINARY_EXPRESSION
        if (node->getToken() == NULL) {
            this->codeGen(node);
            this->outputFile << "PRINT" << std::endl;
            continue;
        }
        switch (node->getToken()->getTokenType()->getTokenKind()) {
            // INT_LITERAL
            case TokenKind::TK_NUM:
                this->outputFile << "PUSH_INT " << node->getToken()->getValue() << std::endl;
                break;
            // STRING_LITERAL
            case TokenKind::TK_STRING:
                this->outputFile << "PUSH_STRING " << node->getToken()->getValue() << std::endl;
                break;
            // VAR_NAME
            case TokenKind::TK_IDENT:
                switch (this->varTable->getType(this->varTable->getAddress(node->getToken()->getValue()))) {
                    case TokenDetail::DEF_INT32:
                        this->outputFile << "LOAD_INT " << this->varTable->getAddress(node->getToken()->getValue()) << std::endl;
                        break;
                    case TokenDetail::DEF_STRING:
                        this->outputFile << "LOAD_STRING " << this->varTable->getAddress(node->getToken()->getValue()) << std::endl;
                        break;
                    default:
                        break;
                }
                break;
            default:
                this->codeGen(node->getChildren()[0]);
                break;
        }
        this->outputFile << "PRINT" << std::endl;
    }
}

void CodeGenerator::storeValue(SyntaxTreeNode* node) {
    switch (node->getToken()->getTokenType()->getTokenKind()) {
        case TokenKind::TK_NUM:
            this->outputFile << "PUSH_INT " << node->getToken()->getValue() << std::endl;
            break;
        case TokenKind::TK_STRING:
            this->outputFile << "PUSH_STRING " << node->getToken()->getValue() << std::endl;
            break;
        case TokenKind::TK_IDENT:
            switch (this->varTable->getType(this->varTable->getAddress(node->getToken()->getValue()))) {
                case TokenDetail::DEF_INT32:
                    this->outputFile << "LOAD_INT " << this->varTable->getAddress(node->getToken()->getValue()) << std::endl;
                    break;
                case TokenDetail::DEF_STRING:
                    this->outputFile << "LOAD_STRING " << this->varTable->getAddress(node->getToken()->getValue()) << std::endl;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return;
}
