#include "code_generator.h"

CodeGenerator::CodeGenerator(SyntaxTree* root, std::string outputDumpFileName, std::string outputRuntimeFileName) {
    this->root = root;
    this->outputDumpFile.open(outputDumpFileName);
    this->outputRuntimeFile.open(outputRuntimeFileName, std::ios::binary);
    this->outputOpCOdeFile.open(OUTPUT_OPCODE_FILE, std::ios::out);
    #if DEBUG
        this->logOpCodes();
    #endif
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
    this->outputOpCOdeFile.close();
    delete this->varTable;
    delete this->functionTable;
}

void CodeGenerator::run(void) {
    this->codeGen(this->root->getRoot(), 0);
    this->writeAddress((int) this->mainFunctionAddress, this->mainCallAddress);

    return;
}

void CodeGenerator::codeGen(SyntaxTreeNode* node, int whileDepth) {
    SyntaxTreeNode* tmpNode;
    std::vector<SyntaxTreeNode*>::iterator itr;
    std::vector<SyntaxTreeNode*>::iterator itr_end;
    std::vector<SyntaxTreeNode*> childNodes;
    std::vector<SyntaxTreeNode*> _childNodes;
    std::vector<int> jumpToEndInsutructionAddress;
    TokenDetail td;
    int varAddress;
    int funcAddress;

    switch (node->getType()) {
        case ASTNodeType::PROGRAM:
            for (SyntaxTreeNode* child : node->getChildren()) {
                this->codeGen(child, whileDepth);
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
                this->codeGen(child, whileDepth);
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
                case TokenDetail::DEF_FLOAT32:
                    this->varTable->add(tmpNode->getToken()->getValue(), TokenDetail::DEF_FLOAT32);
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
                this->codeGen(tmpNode, whileDepth);
                switch (this->varTable->getType(varAddress)) {
                        case TokenDetail::DEF_INT32:
                            this->outputDumpFile << "STORE_INT " << varAddress << std::endl;
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_INT);
                            this->writeIntData(varAddress);
                            break;
                        case TokenDetail::DEF_FLOAT32:
                            this->outputDumpFile << "STORE_FLOAT " << varAddress << std::endl;
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_FLOAT);
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
            // INT_LITERAL or BOOLEAN_LITERAL or STRING_LITERAL or VAR_NAME
            switch (tmpNode->getToken()->getTokenType()->getTokenKind()) {
                // INT_LITERAL
                case TokenKind::TK_NUM:
                    switch (tmpNode->getToken()->getTokenType()->getTokenDetail()) {
                        case TokenDetail::DEF_INT32:
                            this->outputDumpFile << "PUSH_INT " << tmpNode->getToken()->getValue() << std::endl;
                            this->outputDumpFile << "STORE_INT " << varAddress << std::endl;
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_INT);
                            this->writeIntData(stoi(tmpNode->getToken()->getValue()));
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_INT);
                            this->writeIntData(varAddress);
                            break;
                        case TokenDetail::DEF_FLOAT32:
                            this->outputDumpFile << "PUSH_FLOAT " << tmpNode->getToken()->getValue() << std::endl;
                            this->outputDumpFile << "STORE_FLOAT " << varAddress << std::endl;
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_FLOAT);
                            this->writeFloatData(stof(tmpNode->getToken()->getValue()));
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_FLOAT);
                            this->writeIntData(varAddress);
                            break;
                        default:
                            break;
                    }
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
                        case TokenDetail::DEF_FLOAT32:
                            this->outputDumpFile << "LOAD_FLOAT " << this->varTable->getAddress(tmpNode->getToken()->getValue()) << std::endl;
                            this->outputDumpFile << "STORE_FLOAT " << varAddress << std::endl;
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LOAD_FLOAT);
                            this->writeIntData(this->varTable->getAddress(tmpNode->getToken()->getValue()));
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_FLOAT);
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
                    // BOOLEAN_LITERAL
                    td = tmpNode->getToken()->getTokenType()->getTokenDetail();
                    switch (td) {
                        case TokenDetail::TRUE:
                            this->outputDumpFile << "PUSH_INT TRUE" << std::endl;
                            this->outputDumpFile << "STORE_INT " << varAddress << std::endl;
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_INT);
                            this->writeIntData(1);
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_INT);
                            this->writeIntData(varAddress);
                            break;
                        case TokenDetail::FALSE:
                            this->outputDumpFile << "PUSH_INT FALSE" << std::endl;
                            this->outputDumpFile << "STORE_INT " << varAddress << std::endl;
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_INT);
                            this->writeIntData(0);
                            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::STORE_INT);
                            this->writeIntData(varAddress);
                            break;
                        default:
                            break;
                    }
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
                this->printInstructions(tmpNode->getChildren(), whileDepth);
            }
            // println
            else if (tmpNode->getToken()->getValue() == "println") {
                tmpNode = *itr;
                this->printInstructions(tmpNode->getChildren(), whileDepth);
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
                    case ASTNodeType::FLOAT_LITERAL:
                    case ASTNodeType::STRING_LITERAL:
                    case ASTNodeType::BOOLEAN_LITERAL:
                        this->storeValue(childNode);

                        _childNodes = childNode->getChildren();
                        for (SyntaxTreeNode* _childNode: _childNodes) {
                            if (_childNode->getType() == ASTNodeType::OPERATOR) {
                                td = _childNode->getToken()->getTokenType()->getTokenDetail();
                                this->writeOperator(td);
                            }
                            else if (_childNode->getType() == ASTNodeType::VAR_NAME ||
                                    _childNode->getType() == ASTNodeType::INT_LITERAL ||
                                    _childNode->getType() == ASTNodeType::FLOAT_LITERAL ||
                                    _childNode->getType() == ASTNodeType::STRING_LITERAL ||
                                    _childNode->getType() == ASTNodeType::BOOLEAN_LITERAL) {
                                this->storeValue(_childNode);
                            }
                            else {
                                this->codeGen(_childNode, whileDepth);
                            }
                        }
                        break;
                    case ASTNodeType::OPERATOR:
                        td = childNode->getToken()->getTokenType()->getTokenDetail();
                        this->writeOperator(td);
                        break;
                    case ASTNodeType::EXPRESSION:
                        this->codeGen(childNode, whileDepth);
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
        case ASTNodeType::IF_STATEMENT:
            int jumpNextIfInsutructionAddress;
            int previousJumpIfFalseAddress;
            int i;
            int endIfAddress;
            jumpToEndInsutructionAddress.clear();
            childNodes = node->getChildren();
            itr = childNodes.begin();
            itr_end = childNodes.end();

            i = 0;
            while (itr != itr_end) {
                tmpNode = *itr;
                // EXPRESSION
                this->codeGen(tmpNode, whileDepth);
                this->outputDumpFile << "JUMP_IF_FALSE " << jumpNextIfInsutructionAddress << std::endl;
                this->outputRuntimeFile << static_cast<uint8_t>(OpCode::JUMP_IF_FALSE);
                if (i > 0) {
                    this->writeAddress(jumpNextIfInsutructionAddress, previousJumpIfFalseAddress);
                }
                previousJumpIfFalseAddress = (int) this->outputRuntimeFile.tellp();
                this->writeIntData(-1);
                itr++;
                tmpNode = *itr;
                // BLOCK
                for (SyntaxTreeNode* child : tmpNode->getChildren()) {
                    this->codeGen(child, whileDepth);
                }
                this->outputDumpFile << "JUMP TO ENDBLOCK" << std::endl;
                this->outputRuntimeFile << static_cast<uint8_t>(OpCode::JUMP);
                jumpToEndInsutructionAddress.push_back((int) this->outputRuntimeFile.tellp());
                this->writeIntData(-1);
                jumpNextIfInsutructionAddress = (int) this->outputRuntimeFile.tellp();
                itr++;
                i++;
            }
            endIfAddress = (int) this->outputRuntimeFile.tellp();
            for (int address : jumpToEndInsutructionAddress) {
                this->writeAddress(endIfAddress, address);
            }
            this->writeAddress(endIfAddress, previousJumpIfFalseAddress);
            break;
        case ASTNodeType::WHILE_STATEMENT:
            int jumpWhileExitAddress;
            int jumpContinueWhileInsutructionAddress;
            int callJumpToWhileExitAddress;
            jumpToEndInsutructionAddress.clear();
            childNodes = node->getChildren();
            itr = childNodes.begin();
            itr_end = childNodes.end();

            tmpNode = *itr;
            // EXPRESSION
            jumpContinueWhileInsutructionAddress = (int) this->outputRuntimeFile.tellp();
            this->codeGen(tmpNode, whileDepth);
            this->outputDumpFile << "JUMP_IF_FALSE TO ENDBLOCK" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::JUMP_IF_FALSE);
            callJumpToWhileExitAddress = (int) this->outputRuntimeFile.tellp();
            this->writeIntData(-1);
            itr++;
            tmpNode = *itr;
            // BLOCK
            for (SyntaxTreeNode* child : tmpNode->getChildren()) {
                this->codeGen(child, whileDepth + 1);
            }
            this->outputDumpFile << "JUMP TO EXPRESSION" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::JUMP);
            // set jump to while expression address
            this->writeAddress(jumpContinueWhileInsutructionAddress, (int) this->outputRuntimeFile.tellp());
            // set jump to while exit address
            jumpWhileExitAddress = (int) this->outputRuntimeFile.tellp();
            this->writeAddress(jumpWhileExitAddress, callJumpToWhileExitAddress);
            // set break address
            for (int address : this->whileBreakAddressStack[whileDepth + 1]) {
                this->writeAddress(jumpWhileExitAddress, address);
            }
            this->whileBreakAddressStack[whileDepth + 1].clear();
            // set continue address
            for (int address : this->whileContinueAddressStack[whileDepth + 1]) {
                this->writeAddress(jumpContinueWhileInsutructionAddress, address);
            }
            this->whileContinueAddressStack[whileDepth + 1].clear();
            break;
        case ASTNodeType::BREAK:
            this->outputDumpFile << "JUMP TO ENDBLOCK" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::JUMP);
            this->whileBreakAddressStack[whileDepth].push_back((int) this->outputRuntimeFile.tellp());
            this->writeIntData(-1);
            break;
        case ASTNodeType::CONTINUE:
            this->outputDumpFile << "JUMP TO EXPRESSION" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::JUMP);
            this->whileContinueAddressStack[whileDepth].push_back((int) this->outputRuntimeFile.tellp());
            this->writeIntData(-1);
            break;
        default:
            break;
    }
}

void CodeGenerator::printInstructions(std::vector<SyntaxTreeNode*> nodes, int whileDepth) {
    for (SyntaxTreeNode* node : nodes) {
        // EXPRESSION
        if (node->getToken() == NULL) {
            this->codeGen(node, whileDepth);
            this->outputDumpFile << "PRINT" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PRINT);
            continue;
        }
        switch (node->getToken()->getTokenType()->getTokenKind()) {
            // NUM_LITERAL
            case TokenKind::TK_NUM:
                switch (node->getToken()->getTokenType()->getTokenDetail()) {
                    case TokenDetail::DEF_INT32:
                        this->outputDumpFile << "PUSH_INT " << node->getToken()->getValue() << std::endl;
                        this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_INT);
                        this->writeIntData(stoi(node->getToken()->getValue()));
                        break;
                    case TokenDetail::DEF_FLOAT32:
                        this->outputDumpFile << "PUSH_FLOAT " << node->getToken()->getValue() << std::endl;
                        this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_FLOAT);
                        this->writeFloatData(stof(node->getToken()->getValue()));
                        break;
                    default:
                        break;
                }
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
                    case TokenDetail::DEF_FLOAT32:
                        this->outputDumpFile << "LOAD_FLOAT " << this->varTable->getAddress(node->getToken()->getValue()) << std::endl;
                        this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LOAD_FLOAT);
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
                // BOOLEAN_LITERAL
                if (node->getToken()->getTokenType()->getTokenDetail() == TokenDetail::TRUE) {
                    this->outputDumpFile << "PUSH_INT TRUE" << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_INT);
                    this->writeIntData(1);
                }
                else if (node->getToken()->getTokenType()->getTokenDetail() == TokenDetail::FALSE) {
                    this->outputDumpFile << "PUSH_INT FALSE" << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_INT);
                    this->writeIntData(0);
                }
                else {
                    this->codeGen(node->getChildren()[0], whileDepth);
                }
                break;
        }
        this->outputDumpFile << "PRINT" << std::endl;
        this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PRINT);
    }
}

void CodeGenerator::storeValue(SyntaxTreeNode* node) {
    switch (node->getToken()->getTokenType()->getTokenKind()) {
        case TokenKind::TK_NUM:
            switch (node->getToken()->getTokenType()->getTokenDetail()) {
                case TokenDetail::DEF_INT32:
                    this->outputDumpFile << "PUSH_INT " << node->getToken()->getValue() << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_INT);
                    this->writeIntData(stoi(node->getToken()->getValue()));
                    break;
                case TokenDetail::DEF_FLOAT32:
                    this->outputDumpFile << "PUSH_FLOAT " << node->getToken()->getValue() << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_FLOAT);
                    this->writeFloatData(stof(node->getToken()->getValue()));
                    break;
                default:
                    break;
            }
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
                case TokenDetail::DEF_FLOAT32:
                    this->outputDumpFile << "LOAD_FLOAT " << this->varTable->getAddress(node->getToken()->getValue()) << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::LOAD_FLOAT);
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
            switch (node->getToken()->getTokenType()->getTokenDetail()) {
                case TokenDetail::TRUE:
                    this->outputDumpFile << "PUSH_INT TRUE" << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_INT);
                    this->writeIntData(1);
                    break;
                case TokenDetail::FALSE:
                    this->outputDumpFile << "PUSH_INT FALSE" << std::endl;
                    this->outputRuntimeFile << static_cast<uint8_t>(OpCode::PUSH_INT);
                    this->writeIntData(0);
                    break;
                default:
                    break;
            }
            break;
    }

    return;
}

void CodeGenerator::writeIntData(int value) {
    int8_t byte;

    for (long unsigned int i = 0; i < sizeof(int); i++) {
        byte = static_cast<int8_t>(value >> (i * 8));
        this->outputRuntimeFile.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
    }

    return;
}

void CodeGenerator::writeFloatData(float value) {
    int8_t bytes[sizeof(float)];

    std::memcpy(bytes, &value, sizeof(float));
    this->outputRuntimeFile.write(reinterpret_cast<const char*>(bytes), sizeof(float));

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
        case TokenDetail::MOD:
            this->outputDumpFile << "MOD" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::MOD);
            break;
        case TokenDetail::AND:
            this->outputDumpFile << "AND" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::AND);
            break;
        case TokenDetail::OR:
            this->outputDumpFile << "OR" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::OR);
            break;
        case TokenDetail::NOT:
            this->outputDumpFile << "NOT" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::NOT);
            break;
        case TokenDetail::CMP_EQUAL:
            this->outputDumpFile << "CMP_EQUAL" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::CMP_EQUAL);
            break;
        case TokenDetail::CMP_NOT_EQUAL:
            this->outputDumpFile << "CMP_NOT_EQUAL" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::CMP_NOT_EQUAL);
            break;
        case TokenDetail::LEFT_ANGLE_BRACKET:
            this->outputDumpFile << "CMP_LESS" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::CMP_LESS);
            break;
        case TokenDetail::SMALLER_EQUAL:
            this->outputDumpFile << "CMP_LESS_EQUAL" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::CMP_LESS_EQUAL);
            break;
        case TokenDetail::RIGHT_ANGLE_BRACKET:
            this->outputDumpFile << "CMP_GREATER" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::CMP_GREATER);
            break;
        case TokenDetail::LARGER_EQUAL:
            this->outputDumpFile << "CMP_GREATER_EQUAL" << std::endl;
            this->outputRuntimeFile << static_cast<uint8_t>(OpCode::CMP_GREATER_EQUAL);
            break;
        default:
            break;
    }

    return;
}

void CodeGenerator::writeAddress(int value, int writeAddress) {
    this->outputRuntimeFile.seekp(writeAddress);
    this->writeIntData(value);
    this->outputRuntimeFile.seekp(0, std::ios::end);

    return;
}

void CodeGenerator::logOpCodes(void) {
    std::stringstream ss;
    this->outputOpCOdeFile << "OpCodes:" << std::endl;
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::PUSH_INT);
    this->outputOpCOdeFile << ss.str() << ": PUSH_INT" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::PUSH_FLOAT);
    this->outputOpCOdeFile << ss.str() << ": PUSH_FLOAT" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::PUSH_STRING);
    this->outputOpCOdeFile << ss.str() << ": PUSH_STRING" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::LOAD_INT);
    this->outputOpCOdeFile << ss.str() << ": LOAD_INT" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::LOAD_FLOAT);
    this->outputOpCOdeFile << ss.str() << ": LOAD_FLOAT" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::LOAD_STRING);
    this->outputOpCOdeFile << ss.str()<< ": LOAD_STRING" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::STORE_INT);
    this->outputOpCOdeFile << ss.str() << ": STORE_INT" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::STORE_FLOAT);
    this->outputOpCOdeFile << ss.str()<< ": STORE_FLOAT" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::STORE_STRING);
    this->outputOpCOdeFile << ss.str()<< ": STORE_STRING" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::ADD);
    this->outputOpCOdeFile << ss.str() << ": ADD" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::SUB);
    this->outputOpCOdeFile << ss.str() << ": SUB" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::MUL);
    this->outputOpCOdeFile << ss.str() << ": MUL" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::DIV);
    this->outputOpCOdeFile << ss.str() << ": DIV" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::MOD);
    this->outputOpCOdeFile << ss.str() << ": MOD" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::AND);
    this->outputOpCOdeFile << ss.str() << ": AND" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::OR);
    this->outputOpCOdeFile << ss.str() << ": OR" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::NOT);
    this->outputOpCOdeFile << ss.str() << ": NOT" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::CMP_EQUAL);
    this->outputOpCOdeFile << ss.str() << ": CMP_EQUAL" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::CMP_NOT_EQUAL);
    this->outputOpCOdeFile << ss.str() << ": CMP_NOT_EQUAL" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::CMP_LESS);
    this->outputOpCOdeFile << ss.str() << ": CMP_LESS" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::CMP_LESS_EQUAL);
    this->outputOpCOdeFile << ss.str() << ": CMP_LESS_EQUAL" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::CMP_GREATER);
    this->outputOpCOdeFile << ss.str() << ": CMP_GREATER" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::CMP_GREATER_EQUAL);
    this->outputOpCOdeFile << ss.str() << ": CMP_GREATER_EQUAL" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::PRINT);
    this->outputOpCOdeFile << ss.str() << ": PRINT" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::RETURN);
    this->outputOpCOdeFile << ss.str() << ": RETURN" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::JUMP);
    this->outputOpCOdeFile << ss.str() << ": JUMP" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::HALT);
    this->outputOpCOdeFile << ss.str() << ": HALT" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::LABEL);
    this->outputOpCOdeFile << ss.str() << ": LABEL" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::CALL);
    this->outputOpCOdeFile << ss.str() << ": CALL" << std::endl;
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(OpCode::JUMP_IF_FALSE);
    this->outputOpCOdeFile << ss.str() << ": JUMP_IF_FALSE" << std::endl;

    return;
}
