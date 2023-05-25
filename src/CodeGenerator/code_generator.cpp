#include "code_generator.h"

CodeGenerator::CodeGenerator(SyntaxTree* root, std::string outputDumpFileName, std::string outputRuntimeFileName) {
    this->root = root;
    this->outputDumpFile.open(outputDumpFileName);
    this->outputRuntimeFile.open(outputRuntimeFileName, std::ios::binary);
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
    this->outputRuntimeFile.seekp(this->mainCallAddress);
    this->writeIntData((int) this->mainFunctionAddress);
}

void CodeGenerator::codeGen(SyntaxTreeNode* node) {
    SyntaxTreeNode* tmpNode;
    std::vector<SyntaxTreeNode*>::iterator itr;
    std::vector<SyntaxTreeNode*>::iterator itr_end;
    std::vector<SyntaxTreeNode*> childNodes;
    std::vector<SyntaxTreeNode*> _childNodes;
    TokenDetail td;
    int varAddress;
    int funcAddress;

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
            this->functionTable->add(tmpNode->getToken()->getValue(), (int) this->outputRuntimeFile.tellp());
            funcAddress = this->functionTable->getIndex(tmpNode->getToken()->getValue());
            this->outputDumpFile << "LABEL " << funcAddress << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LABEL);
            this->writeIntData(funcAddress);
            this->mainFunctionAddress = this->outputRuntimeFile.tellp();
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
            tmpNode = *itr;
            // EXPRESSION
            if (tmpNode->getType() == ASTNodeType::EXPRESSION) {
                this->codeGen(tmpNode);
                switch (this->varTable->getType(varAddress)) {
                        case TokenDetail::DEF_INT32:
                            this->outputDumpFile << "STORE_INT " << varAddress << std::endl;
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_INT);
                            this->writeIntData(varAddress);
                            break;
                        case TokenDetail::DEF_STRING:
                            this->outputDumpFile << "STORE_STRING " << varAddress << std::endl;
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_STRING);
                            this->writeIntData(varAddress);
                            break;
                        default:
                            break;
                    }
                break;
            }
            // INT_LITERAL or STRING_LITERAL or VAR_NAME
            switch (tmpNode->getToken()->getTokenType()->getTokenKind()) {
                // INT_LITERAL
                case TokenKind::TK_NUM:
                    this->outputDumpFile << "PUSH_INT " << tmpNode->getToken()->getValue() << std::endl;
                    this->outputDumpFile << "STORE_INT " << varAddress << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_INT);
                    this->writeIntData(stoi(tmpNode->getToken()->getValue()));
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_INT);
                    this->writeIntData(varAddress);
                    break;
                // STRING_LITERAL
                case TokenKind::TK_STRING:
                    this->outputDumpFile << "PUSH_STRING " << (int) tmpNode->getToken()->getValue().length() << " " << tmpNode->getToken()->getValue() << std::endl;
                    this->outputDumpFile << "STORE_STRING " << varAddress << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_STRING);
                    this->writeIntData(tmpNode->getToken()->getValue().length());
                    this->outputRuntimeFile << tmpNode->getToken()->getValue();
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_STRING);
                    this->writeIntData(varAddress);
                    break;
                // VAR_NAME
                case TokenKind::TK_IDENT:
                    switch (this->varTable->getType(varAddress)) {
                        case TokenDetail::DEF_INT32:
                            this->outputDumpFile << "LOAD_INT " << this->varTable->getAddress(tmpNode->getToken()->getValue()) << std::endl;
                            this->outputDumpFile << "STORE_INT " << varAddress << std::endl;
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LOAD_INT);
                            this->writeIntData(this->varTable->getAddress(tmpNode->getToken()->getValue()));
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_INT);
                            this->writeIntData(varAddress);
                            break;
                        case TokenDetail::DEF_STRING:
                            this->outputDumpFile << "LOAD_STRING " << this->varTable->getAddress(tmpNode->getToken()->getValue()) << std::endl;
                            this->outputDumpFile << "STORE_STRING " << varAddress << std::endl;
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LOAD_STRING);
                            this->writeIntData(this->varTable->getAddress(tmpNode->getToken()->getValue()));
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_STRING);
                            this->writeIntData(varAddress);
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
                this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_STRING);
                this->writeIntData(1);
                this->outputRuntimeFile << "\n";
                this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PRINT);
            }
            else {
                funcAddress = this->functionTable->getAddress(tmpNode->getToken()->getValue());
                this->outputDumpFile << "CALL " << funcAddress << std::endl;
                this->outputRuntimeFile << static_cast<uint8_t>(OpCode::CALL);
                this->mainCallAddress = this->outputRuntimeFile.tellp();
                this->writeIntData(funcAddress);
                if (tmpNode->getToken()->getValue() == "main") {
                    this->outputDumpFile << "HALT" << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::HALT);
                }
            }
            break;
        case ASTNodeType::EXPRESSION:
            childNodes = node->getChildren();
            for (SyntaxTreeNode* childNode : childNodes) {
                switch (childNode->getType()) {
                    case ASTNodeType::VAR_NAME:
                    case ASTNodeType::INT_LITERAL:
                    case ASTNodeType::STRING_LITERAL:
                        this->storeValue(childNode);
                        _childNodes = childNode->getChildren();
                        for (SyntaxTreeNode* _childNode: _childNodes) {
                            if (_childNode->getType() == ASTNodeType::OPERATOR) {
                                td = _childNode->getToken()->getTokenType()->getTokenDetail();
                                this->writeOperator(td);
                            }
                            else {
                                this->codeGen(_childNode);
                            }
                        }
                        break;
                    case ASTNodeType::OPERATOR:
                        td = childNode->getToken()->getTokenType()->getTokenDetail();
                        this->writeOperator(td);
                        break;
                    case ASTNodeType::EXPRESSION:
                        this->codeGen(childNode);
                        break;
                    default:
                        break;
                }
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
        // EXPRESSION
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
                this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_INT);
                this->writeIntData(stoi(node->getToken()->getValue()));
                break;
            // STRING_LITERAL
            case TokenKind::TK_STRING:
                this->outputDumpFile << "PUSH_STRING " << (int) node->getToken()->getValue().length() << " " << node->getToken()->getValue() << std::endl;
                this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_STRING);
                this->writeIntData((int) node->getToken()->getValue().length());
                this->outputRuntimeFile << node->getToken()->getValue();
                break;
            // VAR_NAME
            case TokenKind::TK_IDENT:
                switch (this->varTable->getType(this->varTable->getAddress(node->getToken()->getValue()))) {
                    case TokenDetail::DEF_INT32:
                        this->outputDumpFile << "LOAD_INT " << this->varTable->getAddress(node->getToken()->getValue()) << std::endl;
                        this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LOAD_INT);
                        this->writeIntData(this->varTable->getAddress(node->getToken()->getValue()));
                        break;
                    case TokenDetail::DEF_STRING:
                        this->outputDumpFile << "LOAD_STRING " << this->varTable->getAddress(node->getToken()->getValue()) << std::endl;
                        this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LOAD_STRING);
                        this->writeIntData(this->varTable->getAddress(node->getToken()->getValue()));
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
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_INT);
            this->writeIntData(stoi(node->getToken()->getValue()));
            break;
        case TokenKind::TK_STRING:
            this->outputDumpFile << "PUSH_STRING " << (int) node->getToken()->getValue().length() << " " << node->getToken()->getValue() << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_STRING);
            this->writeIntData(node->getToken()->getValue().length());
            this->outputRuntimeFile << node->getToken()->getValue();
            break;
        case TokenKind::TK_IDENT:
            switch (this->varTable->getType(this->varTable->getAddress(node->getToken()->getValue()))) {
                case TokenDetail::DEF_INT32:
                    this->outputDumpFile << "LOAD_INT " << this->varTable->getAddress(node->getToken()->getValue()) << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LOAD_INT);
                    this->writeIntData(this->varTable->getAddress(node->getToken()->getValue()));
                    break;
                case TokenDetail::DEF_STRING:
                    this->outputDumpFile << "LOAD_STRING " << this->varTable->getAddress(node->getToken()->getValue()) << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LOAD_STRING);
                    this->writeIntData(this->varTable->getAddress(node->getToken()->getValue()));
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

void CodeGenerator::writeIntData(int value) {
    uint8_t byte;

    for (long unsigned int i = 0; i < sizeof(int); i++) {
        byte = static_cast<uint8_t>(value >> (i * 8));
        this->outputRuntimeFile.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
    }

    return;
}

void CodeGenerator::writeOperator(TokenDetail td) {
    switch (td) {
        case TokenDetail::PLUS:
            this->outputDumpFile << "ADD" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::ADD);
            break;
        case TokenDetail::MINUS:
            this->outputDumpFile << "SUB" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::SUB);
            break;
        case TokenDetail::TIMES:
            this->outputDumpFile << "MUL" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::MUL);
            break;
        case TokenDetail::DIVISION:
            this->outputDumpFile << "DIV" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::DIV);
            break;
        default:
            break;
    }

    return;
}
