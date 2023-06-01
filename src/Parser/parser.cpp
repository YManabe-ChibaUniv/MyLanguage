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
                case TokenDetail::DEF_FLOAT32:
                case TokenDetail::DEF_STRING:
                    return this->parseVariableDefinition();
                    break;
                case TokenDetail::DEF_FUNCTION:
                    nodes.push_back(parseFunctionDefinition());
                    break;
                case TokenDetail::IF:
                    nodes.push_back(parseIfStatement());
                    break;
                case TokenDetail::WHILE:
                    nodes.push_back(parseWhileStatement());
                    break;
                case TokenDetail::BREAK:
                    this->index++;
                    nodes.push_back(new SyntaxTreeNode(NULL, ASTNodeType::BREAK));
                    break;
                case TokenDetail::CONTINUE:
                    this->index++;
                    nodes.push_back(new SyntaxTreeNode(NULL, ASTNodeType::CONTINUE));
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

SyntaxTreeNode* Parser::parseExpression(void) {
    // Expression = Term ((== | != | < | <= | > | >=) Term)*
    SyntaxTreeNode* node = new SyntaxTreeNode(NULL, ASTNodeType::EXPRESSION);
    SyntaxTreeNode* op, *element, *element2;
    Token* token;
    TokenDetail td;
    int i = 0;
    element = this->parseElement();
    node->addChild(element);

    while (true) {
        td = this->tokens[this->index]->getTokenType()->getTokenDetail();
        switch (td) {
            case TokenDetail::CMP_EQUAL:
            case TokenDetail::CMP_NOT_EQUAL:
            case TokenDetail::LEFT_ANGLE_BRACKET:
            case TokenDetail::SMALLER_EQUAL:
            case TokenDetail::RIGHT_ANGLE_BRACKET:
            case TokenDetail::LARGER_EQUAL:
                if (i == 0) {
                    op = this->parseOperator();
                    element2 = this->parseElement();
                    node->addChild(element2);
                    node->addChild(op);
                }
                else {
                    node->addChild(element->clone());
                    op = this->parseOperator();
                    element2 = this->parseElement();
                    node->addChild(element2);
                    node->addChild(op);
                    token = new Token();
                    token->setTokenType(new TokenType(TokenKind::TK_OPERATOR, TokenDetail::AND));
                    token->setValue("AND");
                    node->addChild(new SyntaxTreeNode(token, ASTNodeType::OPERATOR));
                }
                element = element2;
                break;
            default:
                return node;
        }
        i++;
    }

    return node;
}

SyntaxTreeNode* Parser::parseElement(void) {
    // Element = Term ((+ | -) Term)*
    SyntaxTreeNode* node = this->parseTerm();
    SyntaxTreeNode* op;

    while (this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::PLUS || this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::MINUS) {
        op = this->parseOperator();
        node->addChild(this->parseTerm());
        node->addChild(op);
    }

    return node;
}

SyntaxTreeNode* Parser::parseTerm(void) {
    // Term = Factor ((* | / | MOD) Factor)*
    SyntaxTreeNode* node = this->parseFactor();
    SyntaxTreeNode* op;

    while (this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::TIMES || this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::DIVISION || this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::MOD) {
        op = this->parseOperator();
        node->addChild(this->parseFactor());
        node->addChild(op);
    }

    return node;
}

SyntaxTreeNode* Parser::parseFactor(void) {
    // Factor = ( Expression ) | Number | String | Ident | Function()
    SyntaxTreeNode* node = nullptr;
    Token* token = this->tokens[this->index];
    TokenDetail td = token->getTokenType()->getTokenDetail();
    TokenKind tk = token->getTokenType()->getTokenKind();

    // INT LITERAL
    if (tk == TokenKind::TK_NUM) {
        switch (td) {
            case TokenDetail::DEF_INT32:
                node = new SyntaxTreeNode(token, ASTNodeType::INT_LITERAL);
                break;
            case TokenDetail::DEF_FLOAT32:
                node = new SyntaxTreeNode(token, ASTNodeType::FLOAT_LITERAL);
                break;
            default:
                this->runError(this->tokens[this->index]);
        }
        this->index++;
    }
    // STRING LITERAL
    else if (tk == TokenKind::TK_STRING) {
        node = new SyntaxTreeNode(token, ASTNodeType::STRING_LITERAL);
        this->index++;
    }
    // IDENT
    else if (tk == TokenKind::TK_IDENT) {
        if (this->tokens[this->index + 1]->getTokenType()->getTokenDetail() == TokenDetail::LEFT_PARENTHESE) {
            node = this->parseFunctionCall();
        } else {
            node = new SyntaxTreeNode(token, ASTNodeType::VAR_NAME);
            this->index++;
        }
    }
    // ( EXPRESSION )
    else if (td == TokenDetail::LEFT_PARENTHESE) {
        this->index++;
        node = this->parseExpression();
        if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::RIGHT_PARENTHESE) {
            this->runError(this->tokens[this->index]);
        }
        this->index++;
    }
    else {
        this->runError(this->tokens[this->index]);
    }

    return node;
}

SyntaxTreeNode* Parser::parseOperator(void) {
    SyntaxTreeNode* node = nullptr;
    Token* token = this->tokens[this->index];
    TokenDetail td = token->getTokenType()->getTokenDetail();
    switch (td) {
        case TokenDetail::PLUS:
        case TokenDetail::MINUS:
        case TokenDetail::TIMES:
        case TokenDetail::DIVISION:
        case TokenDetail::MOD:
        case TokenDetail::CMP_EQUAL:
        case TokenDetail::CMP_NOT_EQUAL:
        case TokenDetail::LEFT_ANGLE_BRACKET:
        case TokenDetail::SMALLER_EQUAL:
        case TokenDetail::RIGHT_ANGLE_BRACKET:
        case TokenDetail::LARGER_EQUAL:
            node = new SyntaxTreeNode(token, ASTNodeType::OPERATOR);
            this->index++;
            break;
        default:
            this->runError(this->tokens[this->index]);
            break;
    }

    return node;
}

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
        case TokenDetail::DEF_FLOAT32:
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
    // Expression
    node->addChild(this->parseExpression());
    // ; (SEMICOLON)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::SEMICOLON) {
        this->runError(this->tokens[this->index]);
    }
    this->index++;

    return node;
}

SyntaxTreeNode* Parser::parseBinaryExpression(void) {
    SyntaxTreeNode* node = new SyntaxTreeNode(NULL, ASTNodeType::EXPRESSION);
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
    SyntaxTreeNode* node = new SyntaxTreeNode(NULL, ASTNodeType::ARGUMENT_LIST);

    while (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::RIGHT_PARENTHESE) {
        // Expression
        node->addChild(this->parseExpression());
        // , (COMMA)
        if (this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::COMMA) {
            this->index++;
        }
    }

    return node;
}

SyntaxTreeNode* Parser::parseIfStatement(void) {
    SyntaxTreeNode* node = new SyntaxTreeNode(NULL, ASTNodeType::IF_STATEMENT);
    Token* trueToken;
    SyntaxTreeNode* elseExpression;
    // if (IF)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::IF) {
        this->runError(this->tokens[this->index]);
    }
    this->index++;
    // ( (LEFT_PARENTHESE)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::LEFT_PARENTHESE) {
        this->runError(this->tokens[this->index]);
    }
    this->index++;
    // IFExpression
    node->addChild(this->parseIfExpression());
    // ) (RIGHT_PARENTHESE)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::RIGHT_PARENTHESE) {
        this->runError(this->tokens[this->index]);
    }
    this->index++;
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
    // else (ELSE)
    while (this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::ELSE) {
        this->index++;
        // if (IF)
        if (this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::IF) {
            this->index++;
            // ( (LEFT_PARENTHESE)
            if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::LEFT_PARENTHESE) {
                this->runError(this->tokens[this->index]);
            }
            this->index++;
            // IFExpression
            node->addChild(this->parseIfExpression());
            // ) (RIGHT_PARENTHESE)
            if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::RIGHT_PARENTHESE) {
                this->runError(this->tokens[this->index]);
            }
            this->index++;
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
        } else {
            elseExpression = new SyntaxTreeNode(NULL, ASTNodeType::EXPRESSION);
            trueToken = new Token();
            trueToken->setTokenType(new TokenType(TokenKind::TK_KEYWORD, TokenDetail::TRUE));
            trueToken->setValue("TRUE");
            elseExpression->addChild(new SyntaxTreeNode(trueToken, ASTNodeType::BOOLEAN_LITERAL));
            node->addChild(elseExpression);
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
            break;
        }
    }

    return node;
}

SyntaxTreeNode* Parser::parseIfExpression(void) {
    // Expression = Term (OR Term)*
    SyntaxTreeNode* node = new SyntaxTreeNode(NULL, ASTNodeType::EXPRESSION);
    SyntaxTreeNode* op;

    node->addChild(this->parseIfTerm());
    while (this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::OR) {
        op = this->parseIfOperator();
        node->addChild(this->parseIfTerm());
        node->addChild(op);
    }

    return node;
}

SyntaxTreeNode* Parser::parseIfTerm(void) {
    // Term = Factor (AND Factor)*
    SyntaxTreeNode* node = this->parseIfFactor();
    SyntaxTreeNode* op;

    while (this->tokens[this->index]->getTokenType()->getTokenDetail() == TokenDetail::AND) {
        op = this->parseIfOperator();
        node->addChild(this->parseIfFactor());
        node->addChild(op);
    }

    return node;
}

SyntaxTreeNode* Parser::parseIfFactor(void) {
    // Factor = (NOT)? (TRUE | FALSE | (IFExpression) | Expression)
    SyntaxTreeNode* node = nullptr;
    SyntaxTreeNode* notNode = nullptr;
    Token* token = this->tokens[this->index];
    TokenDetail td = token->getTokenType()->getTokenDetail();

    // (NOT)?
    if (td == TokenDetail::NOT) {
        notNode = new SyntaxTreeNode(token, ASTNodeType::OPERATOR);
        this->index++;
    }
    token = this->tokens[this->index];
    td = token->getTokenType()->getTokenDetail();
    // TRUE | FALSE
    if (td == TokenDetail::TRUE || td == TokenDetail::FALSE) {
        node = new SyntaxTreeNode(token, ASTNodeType::BOOLEAN_LITERAL);
        this->index++;
    }
    // ( IFExpression )
    else if (td == TokenDetail::LEFT_PARENTHESE) {
        this->index++;
        node = this->parseIfExpression();
        // ) (RIGHT_PARENTHESE)
        if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::RIGHT_PARENTHESE) {
            this->runError(this->tokens[this->index]);
        }
        this->index++;
    }
    // Expression
    else {
        node = this->parseExpression();
    }

    if (notNode != nullptr) {
        node->addChild(notNode);
    }

    return node;
}

SyntaxTreeNode* Parser::parseIfOperator(void) {
    SyntaxTreeNode* node = nullptr;
    Token* token = this->tokens[this->index];
    TokenDetail td = token->getTokenType()->getTokenDetail();
    switch (td) {
        case TokenDetail::AND:
        case TokenDetail::OR:
        case TokenDetail::NOT:
            node = new SyntaxTreeNode(token, ASTNodeType::OPERATOR);
            this->index++;
            break;
        default:
            this->runError(this->tokens[this->index]);
            break;
    }

    return node;
}

SyntaxTreeNode* Parser::parseWhileStatement(void) {
    // WhileStatement = WHILE ( (LEFT_PARENTHESE) Expression (RIGHT_PARENTHESE) ) (LEFT_BRACKET) BlockStatement (RIGHT_BRACKET)
    SyntaxTreeNode* node = new SyntaxTreeNode(this->tokens[this->index], ASTNodeType::WHILE_STATEMENT);

    this->index++;
    // ( (LEFT_PARENTHESE)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::LEFT_PARENTHESE) {
        this->runError(this->tokens[this->index]);
    }
    this->index++;
    // Expression
    node->addChild(this->parseIfExpression());
    // ) (RIGHT_PARENTHESE)
    if (this->tokens[this->index]->getTokenType()->getTokenDetail() != TokenDetail::RIGHT_PARENTHESE) {
        this->runError(this->tokens[this->index]);
    }
    this->index++;
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
