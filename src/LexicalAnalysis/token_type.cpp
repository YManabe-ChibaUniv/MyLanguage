#include "token_type.h"

TokenType::TokenType(TokenKind kind, TokenDetail detail) {
    this->kind = kind;
    this->detail = detail;
}

TokenKind TokenType::getTokenKind(void) {
    return this->kind;
}

TokenDetail TokenType::getTokenDetail(void) {
    return this->detail;
}

std::string TokenType::__str(void) {
    std::string str;
    str =  "[";
    switch (this->kind) {
        case TokenKind::TK_KEYWORD:
            str += "TK_KEYWORD";
            break;
        case TokenKind::TK_PUNCTUATOR:
            str += "TK_PUNCTUATOR";
            break;
        case TokenKind::TK_OPERATOR:
            str += "TK_OPERATOR";
            break;
        case TokenKind::TK_IDENT:
            str += "TK_IDENT";
            break;
        case TokenKind::TK_NUM:
            str += "TK_NUM";
            break;
        case TokenKind::TK_STRING:
            str += "TK_STRING";
            break;
        case TokenKind::TK_PROGRAM:
            str += "TK_PROGRAM";
            break;
        case TokenKind::TK_EOF:
            str += "TK_EOF";
            break;
    }
    str += ", ";
    switch (this->detail) {
        case TokenDetail::DEF_FUNCTION:
            str += "DEF_FUNCTION";
            break;
        case TokenDetail::DEF_INT32:
            str += "DEF_INT32";
            break;
        case TokenDetail::DEF_STRING:
            str += "DEF_STRING";
            break;
        case TokenDetail::DEF_VOID:
            str += "DEF_VOID";
            break;
        case TokenDetail::DEF_RETURN:
            str += "DEF_RETURN";
            break;
        case TokenDetail::LEFT_PARENTHESE:
            str += "LEFT_PARENTHESE";
            break;
        case TokenDetail::RIGHT_PARENTHESE:
            str += "RIGHT_PARENTHESE";
            break;
        case TokenDetail::LEFT_BRACKET:
            str += "LEFT_BRACKET";
            break;
        case TokenDetail::RIGHT_BRACKET:
            str += "RIGHT_BRACKET";
            break;
        case TokenDetail::SEMICOLON:
            str += "SEMICOLON";
            break;
        case TokenDetail::COMMA:
            str += "COMMA";
            break;
        case TokenDetail::DOUBLE_QUOTATION:
            str += "DOUBLE_QUOTATION";
            break;
        case TokenDetail::RETURN_TYPE:
            str += "RETURN_TYPE";
            break;
        case TokenDetail::MOD:
            str += "MOD";
            break;
        case TokenDetail::EQUAL:
            str += "EQUAL";
            break;
        case TokenDetail::PLUS:
            str += "PLUS";
            break;
        case TokenDetail::MINUS:
            str += "MINUS";
            break;
        case TokenDetail::TIMES:
            str += "TIMES";
            break;
        case TokenDetail::DIVISION:
            str += "DIVISION";
            break;
        case TokenDetail::LEFT_ANGLE_BRACKET:
            str += "LEFT_ANGLE_BRACKET";
            break;
        case TokenDetail::RIGHT_ANGLE_BRACKET:
            str += "RIGHT_ANGLE_BRACKET";
            break;
        case TokenDetail::NOTHING:
            str += "NOTHING";
            break;
    }
    str += "]";
    return str;
}
