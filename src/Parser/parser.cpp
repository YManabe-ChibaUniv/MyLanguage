#include "parser.h"

Parser::Parser(std::vector<Token*> tokens) {
    this->tokens = tokens;
    this->index = 0;
}

Parser::~Parser() {

}

SyntaxTree* Parser::getTree(void) {
    return this->tree;
}

void Parser::printTree(void) {
    this->tree->printTree();
    return;
}

void Parser::run(void) {
    this->tree = new SyntaxTree(this->parseProgram());

    #if DEBUG
        std::ofstream ofs(SYNTAX_TREE_FILE);
        ofs << "Syntax Tree" << std::endl;
        std::streambuf* strbuf = std::cout.rdbuf(ofs.rdbuf());
        this->tree->printTree();
        std::cout.rdbuf(strbuf);
        ofs.close();
    #endif

    return;
}

SyntaxTreeNode* Parser::parseProgram(void) {
    SyntaxTreeNode* node = new SyntaxTreeNode(NULL, ASTNodeType::PROGRAM);

    // call main
    SyntaxTreeNode* callMainNode = new SyntaxTreeNode(NULL, ASTNodeType::FUNCTION_CALL);
    Token* callMain = new Token();
    callMain->setTokenType(new TokenType(TokenKind::TK_IDENT, TokenDetail::NOTHING));
    callMain->setValue("main");
    callMainNode->addChild(new SyntaxTreeNode(callMain, ASTNodeType::FUNCTION_NAME));
    callMainNode->addChild(new SyntaxTreeNode(NULL, ASTNodeType::ARGUMENT_LIST));
    node->addChild(callMainNode);

    while (this->index < (int)this->tokens.size()) {
        std::vector<SyntaxTreeNode*> nodes = this->parseStatement();
        for (SyntaxTreeNode* _node : nodes) {
            node->addChild(_node);
        }
    }

    return node;
}

std::vector<SyntaxTreeNode*> Parser::parseStatement(void) {
    Token* token = this->tokens[this->index];
    std::vector<SyntaxTreeNode*> nodes;
    switch (token->getTokenType()->getTokenKind()) {
        case TokenKind::TK_KEYWORD:
            switch (token->getTokenType()->getTokenDetail()) {
                case TokenDetail::DEF_RETURN:
                    nodes.push_back(parseReturnStatement());
                    break;
                case TokenDetail::DEF_INT32:
                case TokenDetail::DEF_STRING:
                    return this->parseVariableDefinition();
                    break;
                case TokenDetail::DEF_FUNCTION:
                    nodes.push_back(parseFunctionDefinition());
                    break;
                default:
                    this->runError(this->tokens[this->index]);
            }
            break;
        case TokenKind::TK_IDENT:
            if (this->tokens[this->index + 1]->getTokenType()->getTokenDetail() == TokenDetail::LEFT_PARENTHESE) {
                nodes.push_back(this->parseFunctionCall());
            } else {
                nodes.push_back(this->parseVariableAssignment());
            }
            break;
        case TokenKind::TK_PUNCTUATOR:
            if (this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::SEMICOLON) {
                this->index++;
            }
            /*
            else if (token->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::LEFT_BRACE) {
                nodes.push_back(this->parseBlockStatement());
            }
            */
            else {
                this->runError(this->tokens[this->index]);
            }
            break;
        default:
            this->runError(this->tokens[this->index]);
    }
    return nodes;
}
/*
SyntaxTreeNode* Parser::parseExpression(void) {
    SyntaxTreeNode* node = this->parseTerm();
    while (this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::PLUS || this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::MINUS) {
        node->addChild(this->parseOperator());
        node->addChild(this->parseTerm());
    }
    return node;
}
*/
/*
SyntaxTreeNode* Parser::parseTerm(void) {
    SyntaxTreeNode* node = this->parseFactor();
    while (this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::TIMES || this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::DIVISION) {
        node->addChild(this->parseOperator());
        node->addChild(this->parseFactor());
    }
    return node;
}
*/
/*
SyntaxTreeNode* Parser::parseFactor(void) {
    SyntaxTreeNode* node = nullptr;
    Token* token = this->tokens[this->index];
    TokenDetail td = token->getTokenType()->getTokenDetail();
    TokenKind tk = token->getTokenType()->getTokenKind();

    if (td == TokenDetail::DEF_INT32 || td == TokenDetail::DEF_STRING) {
        node = new SyntaxTreeNode(token, ASTNodeType::VAR_TYPE);
        this->index++;
    }
    else if (tk == TokenKind::TK_IDENT) {
        if (this->tokens[this->index + 1]->getTokenType()->getTokenDetail() == TokenDetail::LEFT_PARENTHESE) {
            node = this->parseFunctionCall();
        } else {
            node = this->parseVariable();
        }
    }
    else if (td == TokenDetail::TIMES) {
        if (this->tokens[this->index + 1]->getTokenType()->getTokenDetail() == TokenDetail::LEFT_PARENTHESE) {
            this->index++;
            node = this->parseExpression();
            if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::RIGHT_PARENTHESE) {
                this->runError(this->tokens[this->index]);
            }
            this->index++;
        } else {
            this->runError(this->tokens[this->index]);
        }
    }
    else {
        this->runError(this->tokens[this->index]);
    }

    return node;
}
*/
/*
SyntaxTreeNode* Parser::parseOperator(void) {
    SyntaxTreeNode* node = nullptr;
    Token* token = this->tokens[this->index];
    TokenDetail td = token->getTokenType()->getTokenDetail();
    if (td == TokenDetail::PLUS || td == TokenDetail::MINUS || td == TokenDetail::TIMES || td == TokenDetail::DIVISION) {
        node = new SyntaxTreeNode(token, ASTNodeType::OPERATOR);
        this->index++;
    } else {
        this->runError(this->tokens[this->index]);
    }
    return node;
}
*/
/*
SyntaxTreeNode* Parser::parseVariable(void) {
    SyntaxTreeNode* node = nullptr;
    Token* token = this->tokens[this->index];
    TokenKind tk = token->getTokenType()->getTokenKind();
    if (tk == TokenKind::TK_IDENT) {
        node = new SyntaxTreeNode(token, ASTNodeType::VAR);
        this->index++;
    } else {
        this->runError(this->tokens[this->index]);
    }
    return node;
}
*/

SyntaxTreeNode* Parser::parseReturnStatement(void) {
    SyntaxTreeNode* node = new SyntaxTreeNode(NULL, ASTNodeType::RETURN_STATEMENT);
    this->index++;
    // ; (TK_SEMICOLON)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::SEMICOLON) {
        this->runError(this->tokens[this->index]);
    }
    return node;
}

SyntaxTreeNode* Parser::parseFunctionCall(void) {
    SyntaxTreeNode* node = new SyntaxTreeNode(NULL, ASTNodeType::FUNCTION_CALL);
    // FUNCTION_NAME (TK_IDENT)
    node->addChild(new SyntaxTreeNode(this->tokens[this->index], ASTNodeType::FUNCTION_NAME));
    this->index++;
    // ( (TK_LEFT_PARENTHESE)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::LEFT_PARENTHESE) {
        this->runError(this->tokens[this->index]);
    }
    this->index++;
    // PARAM_LIST
    node->addChild(this->parseArgumentList());
    // ) (TK_RIGHT_PARENTHESE)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::RIGHT_PARENTHESE) {
        this->runError(this->tokens[this->index]);
    }
    this->index++;
    return node;
}

// node: function
SyntaxTreeNode* Parser::parseFunctionDefinition(void) {
    // FUNCTION (DEF_FUNCTION)
    SyntaxTreeNode* node = new SyntaxTreeNode(NULL, ASTNodeType::FUNCTION_DEFINITION);
    this->index++;
    // FUNCTION_NAME (TK_IDENT)
    if (this->tokens[this->index]->getTokenType()->getTokenKind() != TokenKind::TK_IDENT) {
        this->runError(this->tokens[this->index]);
    }
    node->addChild(new SyntaxTreeNode(this->tokens[this->index], ASTNodeType::FUNCTION_NAME));
    this->index++;
    // ( (LEFT_PARENTHESE)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::LEFT_PARENTHESE) {
        this->runError(this->tokens[this->index]);
    }
    this->index++;
    // PARAM_LIST
    SyntaxTreeNode* paramList = new SyntaxTreeNode(NULL, ASTNodeType::PARAM_LIST);
    Token* token = this->tokens[this->index];
    // VOID (DEF_VOID)
    if (token->getTokenType()->getTokenDetail() == TokenDetail::DEF_VOID) {
        paramList->addChild(new SyntaxTreeNode(token, ASTNodeType::VAR_TYPE));
        this->index++;
    }
    // int32 (DEF_INT32)
    // string (DEF_STRING)
    else if (token->getTokenType()->getTokenDetail() == TokenDetail::DEF_INT32 || token->getTokenType()->getTokenDetail() == TokenDetail::DEF_STRING) {
        while (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::RIGHT_PARENTHESE) {
            Token* varTypeToken = this->tokens[this->index];
            // int32 (DEF_INT32)
            // string (DEF_STRING)
            switch (this->tokens[this->index]->getTokenType()->getTokenDetail()) {
                case TokenDetail::DEF_INT32:
                case TokenDetail::DEF_STRING:
                    break;
                default:
                    this->runError(this->tokens[this->index]);
            }
            this->index++;
            // VAR_NAME (TK_IDENT)
            if (this->tokens[this->index]->getTokenType()->getTokenKind() != TokenKind::TK_IDENT) {
                this->runError(this->tokens[this->index]);
            }
            this->index++;
            paramList->addChild(new SyntaxTreeNode(varTypeToken, ASTNodeType::VAR_TYPE));
            // , (COMMA)
            if (this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::COMMA) {
                this->index++;
            }
        }
    }
    else {
        this->runError(this->tokens[this->index]);
    }
    // ) (RIGHT_PARENTHESE)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::RIGHT_PARENTHESE) {
        this->runError(this->tokens[this->index]);
    }
    this->index++;
    node->addChild(paramList);
    // -> (RETURN_TYPE)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::RETURN_TYPE) {
        this->runError(this->tokens[this->index]);
    }
    this->index++;
    // void (DEF_VOID)
    // int32 (DEF_INT32)
    // string (DEF_STRING)
    switch (this->tokens[this->index]->getTokenType()->getTokenDetail()) {
        case TokenDetail::DEF_VOID:
        case TokenDetail::DEF_INT32:
        case TokenDetail::DEF_STRING:
            node->addChild(new SyntaxTreeNode(this->tokens[this->index], ASTNodeType::RETURN_TYPE));
            this->index++;
            break;
        default:
            this->runError(this->tokens[this->index]);
    }
    // { (LEFT_BRACKET)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::LEFT_BRACKET) {
        this->runError(this->tokens[this->index]);
    }
    // goto BLOCK
    node->addChild(this->parseBlockStatement());
    // } (RIGHT_BRACKET)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::RIGHT_BRACKET) {
        this->runError(this->tokens[this->index]);
    }
    this->index++;
    return node;
}

SyntaxTreeNode* Parser::parseBlockStatement(void) {
    SyntaxTreeNode* node = new SyntaxTreeNode(NULL, ASTNodeType::BLOCK);
    // { (LEFT_BRACKET)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::LEFT_BRACKET) {
        this->runError(this->tokens[this->index]);
    }
    this->index++;
    while (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::RIGHT_BRACKET) {
        std::vector<SyntaxTreeNode*> nodes = this->parseStatement();
        for (SyntaxTreeNode* _node : nodes) {
            node->addChild(_node);
        }
    }
    return node;
}

// parseVariableDefinition
std::vector<SyntaxTreeNode*> Parser::parseVariableDefinition(void) {
    std::vector<SyntaxTreeNode*> nodes;
    Token* varTypeToken;
    // int32 (DEF_INT32)
    // string (DEF_STRING)
    switch (this->tokens[this->index]->getTokenType()->getTokenDetail()) {
        case TokenDetail::DEF_INT32:
        case TokenDetail::DEF_STRING:
            varTypeToken = this->tokens[this->index];
            this->index++;
            while (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::SEMICOLON) {
                // VAR_NAME (TK_IDENT)
                if (this->tokens[this->index]->getTokenType()->getTokenKind() != TokenKind::TK_IDENT) {
                    this->runError(this->tokens[this->index]);
                }
                SyntaxTreeNode* node = new SyntaxTreeNode(NULL, ASTNodeType::VAR_DEFINITION);
                node->addChild(new SyntaxTreeNode(varTypeToken, ASTNodeType::VAR_TYPE));
                node->addChild(new SyntaxTreeNode(this->tokens[this->index], ASTNodeType::VAR_NAME));
                nodes.push_back(node);
                this->index++;
                // , (COMMA)
                if (this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::COMMA) {
                    this->index++;
                }
            }
            this->index++;
            break;
        default:
            this->runError(this->tokens[this->index]);
    }
    return nodes;
}

SyntaxTreeNode* Parser::parseVariableAssignment(void) {
    SyntaxTreeNode* node = new SyntaxTreeNode(NULL, ASTNodeType::ASSIGNMENT_EXPRESSION);
    // VAR_NAME (TK_IDENT)
    node->addChild(new SyntaxTreeNode(this->tokens[this->index], ASTNodeType::VAR_NAME));
    this->index++;
    // = (EQUAL)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::EQUAL) {
        this->runError(this->tokens[this->index]);
    }
    this->index++;
    // NUMBER (TK_NUM)
    // STRING (TK_STRING)
    // VAR_NAME (TK_IDENT)
    switch (this->tokens[this->index]->getTokenType()->getTokenKind()) {
        case TokenKind::TK_NUM:
            node->addChild(new SyntaxTreeNode(this->tokens[this->index], ASTNodeType::INT_LITERAL));
            this->index++;
            break;
        case TokenKind::TK_STRING:
            node->addChild(new SyntaxTreeNode(this->tokens[this->index], ASTNodeType::STRING_LITERAL));
            this->index++;
            break;
        case TokenKind::TK_IDENT:
            node->addChild(new SyntaxTreeNode(this->tokens[this->index], ASTNodeType::VAR_NAME));
            this->index++;
            break;
        default:
            this->runError(this->tokens[this->index]);
    }
    // ; (SEMICOLON)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::SEMICOLON) {
        this->runError(this->tokens[this->index]);
    }
    this->index++;

    return node;
}

SyntaxTreeNode* Parser::parseBinaryExpression(void) {
    SyntaxTreeNode* node = new SyntaxTreeNode(NULL, ASTNodeType::BINARY_EXPRESSION);
    // VAR_NAME (TK_IDENT)
    if (this->tokens[this->index]->getTokenType()->getTokenKind() != TokenKind::TK_IDENT) {
        this->runError(this->tokens[this->index]);
    }
    node->addChild(new SyntaxTreeNode(this->tokens[this->index], ASTNodeType::VAR_NAME));
    this->index++;
    // + (PLUS)
    // - (MINUS)
    switch (this->tokens[this->index]->getTokenType()->getTokenDetail()) {
        case TokenDetail::PLUS:
        case TokenDetail::MINUS:
            node->addChild(new SyntaxTreeNode(this->tokens[this->index], ASTNodeType::OPERATOR));
            this->index++;
            break;
        default:
            this->runError(this->tokens[this->index]);
    }
    // VAR_NAME (TK_IDENT)
    if (this->tokens[this->index]->getTokenType()->getTokenKind() != TokenKind::TK_IDENT) {
        this->runError(this->tokens[this->index]);
    }
    node->addChild(new SyntaxTreeNode(this->tokens[this->index], ASTNodeType::VAR_NAME));
    this->index++;

    return node;
}

SyntaxTreeNode* Parser::parseArgumentList(void) {
    TokenDetail next;

    SyntaxTreeNode* node = new SyntaxTreeNode(NULL, ASTNodeType::ARGUMENT_LIST);
    bool isBinaryExpression;
    while (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::RIGHT_PARENTHESE) {
        isBinaryExpression = false;
        // NUMBER (TK_NUM)
        // STRING (TK_STRING)
        // VAR_NAME (TK_IDENT)
        switch (this->tokens[this->index]->getTokenType()->getTokenKind()) {
            case TokenKind::TK_NUM:
                next = this->tokens[this->index + 1]->getTokenType()->getTokenDetail();
                if (next != TokenDetail::PLUS && next != TokenDetail::MINUS) {
                    node->addChild(new SyntaxTreeNode(this->tokens[this->index], ASTNodeType::INT_LITERAL));
                    this->index++;
                }
                else {
                    isBinaryExpression = true;
                }
                break;
            case TokenKind::TK_STRING:
                next = this->tokens[this->index + 1]->getTokenType()->getTokenDetail();
                if (next!= TokenDetail::PLUS && next != TokenDetail::MINUS) {
                    node->addChild(new SyntaxTreeNode(this->tokens[this->index], ASTNodeType::STRING_LITERAL));
                    this->index++;
                }
                else {
                    isBinaryExpression = true;
                }
                break;
            case TokenKind::TK_IDENT:
                next = this->tokens[this->index + 1]->getTokenType()->getTokenDetail();
                if (next!= TokenDetail::PLUS && next != TokenDetail::MINUS) {
                    node->addChild(new SyntaxTreeNode(this->tokens[this->index], ASTNodeType::VAR_NAME));
                    this->index++;
                }
                else {
                    isBinaryExpression = true;
                }
                break;
            default:
                this->runError(this->tokens[this->index]);
        }
        // + (PLUS)
        // - (MINUS)
        if (isBinaryExpression) {
            node->addChild(this->parseBinaryExpression());
        }
        // , (COMMA)
        if (this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::COMMA) {
            this->index++;
        }
    }

    return node;
}

void Parser::runError(Token* token, const std::experimental::fundamentals_v2::source_location& location) {
    std::cerr << "Syntax Error [" << location.file_name() << ":" << location.line() << ":" << location.function_name() << "] at " << this->index << " : ";
    if (token == NULL) {
        std::cerr << "Unexpected EOF" << std::endl;
    }
    else {
        std::cerr << token->getTokenType()->__str() << " " << token->getValue() << std::endl;
    }
    throw "";
}
