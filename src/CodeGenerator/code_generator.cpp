#include "code_generator.h"

CodeGenerator::CodeGenerator(SyntaxTree* root, std::string outputDumpFileName, std::string outputRuntimeFileName) {
    this->root = root;
    this->outputDumpFile.open(outputDumpFileName);
    this->outputRuntimeFile.open(outputRuntimeFileName, std::ios::out | std::ios::trunc | std::ios::binary);
    this->varTable = new VarTable();
    this->functionTable = new FunctionTable();
}

CodeGenerator::~CodeGenerator() {
    for (Instruction* i : this->instructions) {
        delete i;
    }
    this->instructions.clear();
    this->outputDumpFile.close();
    this->outputRuntimeFile.close();
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
            this->functionTable->add(tmpNode->getToken()->getValue(), this->outputRuntimeFile.tellp());
            this->outputDumpFile << "LABEL " << this->functionTable->getAddress(tmpNode->getToken()->getValue()) << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LABEL) << this->functionTable->getAddress(tmpNode->getToken()->getValue());
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
                    this->outputDumpFile << "PUSH_INT " << tmpNode->getToken()->getValue() << std::endl;
                    this->outputDumpFile << "STORE_INT " << varAddress << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_INT) << stoi(tmpNode->getToken()->getValue());
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_INT) << varAddress;
                    break;
                // STRING_LITERAL
                case TokenKind::TK_STRING:
                    this->outputDumpFile << "PUSH_STRING " << tmpNode->getToken()->getValue().length() << " " << tmpNode->getToken()->getValue() << std::endl;
                    this->outputDumpFile << "STORE_STRING " << varAddress << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_INT) << tmpNode->getToken()->getValue().length() << tmpNode->getToken()->getValue();
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_INT) << varAddress;
                    break;
                // VAR_NAME
                case TokenKind::TK_IDENT:
                    switch (this->varTable->getType(varAddress)) {
                        case TokenDetail::DEF_INT32:
                            this->outputDumpFile << "LOAD_INT " << this->varTable->getAddress(tmpNode->getToken()->getValue()) << std::endl;
                            this->outputDumpFile << "STORE_INT " << varAddress << std::endl;
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LOAD_INT) << this->varTable->getAddress(tmpNode->getToken()->getValue());
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_INT) << varAddress;
                            break;
                        case TokenDetail::DEF_STRING:
                            this->outputDumpFile << "LOAD_STRING " << this->varTable->getAddress(tmpNode->getToken()->getValue()) << std::endl;
                            this->outputDumpFile << "STORE_STRING " << varAddress << std::endl;
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LOAD_STRING) << this->varTable->getAddress(tmpNode->getToken()->getValue());
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_STRING) << varAddress;
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
                this->outputDumpFile << "PUSH_STRING " << 1 << " \\n" << std::endl;
                this->outputDumpFile << "PRINT" << std::endl;
                this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_STRING) << (size_t) 1 << "\n";
            }
            else {
                this->outputDumpFile << "JUMP " << this->functionTable->getAddress(tmpNode->getToken()->getValue()) << std::endl;
                this->outputRuntimeFile << static_cast<uint8_t>(OpCode::JUMP) << this->functionTable->getAddress(tmpNode->getToken()->getValue());
                if (tmpNode->getToken()->getValue() == "main") {
                    this->outputDumpFile << "HALT" << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::HALT);
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
                    this->outputDumpFile << "ADD" << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::ADD);
                    break;
                case TokenDetail::MINUS:
                    this->outputDumpFile << "SUB" << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::SUB);
                    break;
                default:
                    break;
            }
            break;
        case ASTNodeType::RETURN_STATEMENT:
            this->outputDumpFile << "RETURN" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::RETURN);
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
            this->outputDumpFile << "PRINT" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PRINT);
            continue;
        }
        switch (node->getToken()->getTokenType()->getTokenKind()) {
            // INT_LITERAL
            case TokenKind::TK_NUM:
                this->outputDumpFile << "PUSH_INT " << node->getToken()->getValue() << std::endl;
                this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_INT) << node->getToken()->getValue();
                break;
            // STRING_LITERAL
            case TokenKind::TK_STRING:
                this->outputDumpFile << "PUSH_STRING " << node->getToken()->getValue().length() << " " << node->getToken()->getValue() << std::endl;
                this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_STRING) << node->getToken()->getValue().length() << node->getToken()->getValue();
                break;
            // VAR_NAME
            case TokenKind::TK_IDENT:
                switch (this->varTable->getType(this->varTable->getAddress(node->getToken()->getValue()))) {
                    case TokenDetail::DEF_INT32:
                        this->outputDumpFile << "LOAD_INT " << this->varTable->getAddress(node->getToken()->getValue()) << std::endl;
                        this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LOAD_INT) << this->varTable->getAddress(node->getToken()->getValue());
                        break;
                    case TokenDetail::DEF_STRING:
                        this->outputDumpFile << "LOAD_STRING " << this->varTable->getAddress(node->getToken()->getValue()) << std::endl;
                        this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LOAD_STRING) << this->varTable->getAddress(node->getToken()->getValue());
                        break;
                    default:
                        break;
                }
                break;
            default:
                this->codeGen(node->getChildren()[0]);
                break;
        }
        this->outputDumpFile << "PRINT" << std::endl;
        this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PRINT);
    }
}

void CodeGenerator::storeValue(SyntaxTreeNode* node) {
    switch (node->getToken()->getTokenType()->getTokenKind()) {
        case TokenKind::TK_NUM:
            this->outputDumpFile << "PUSH_INT " << node->getToken()->getValue() << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_INT) << node->getToken()->getValue();
            break;
        case TokenKind::TK_STRING:
            this->outputDumpFile << "PUSH_STRING " << node->getToken()->getValue().length() << " " << node->getToken()->getValue() << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_STRING) << node->getToken()->getValue().length() << node->getToken()->getValue();
            break;
        case TokenKind::TK_IDENT:
            switch (this->varTable->getType(this->varTable->getAddress(node->getToken()->getValue()))) {
                case TokenDetail::DEF_INT32:
                    this->outputDumpFile << "LOAD_INT " << this->varTable->getAddress(node->getToken()->getValue()) << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LOAD_INT) << this->varTable->getAddress(node->getToken()->getValue());
                    break;
                case TokenDetail::DEF_STRING:
                    this->outputDumpFile << "LOAD_STRING " << this->varTable->getAddress(node->getToken()->getValue()) << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LOAD_STRING) << this->varTable->getAddress(node->getToken()->getValue());
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
