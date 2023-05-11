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
