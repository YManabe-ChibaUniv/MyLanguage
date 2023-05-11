#include "token.h"

Token::Token(void) {
    this->type = NULL;
    this->value = "";
}

Token::~Token() {
    delete(this->type);
}

TokenType* Token::getTokenType(void) {
    return this->type;
}

std::string Token::getValue(void) {
    return this->value;
}

void Token::setTokenType(TokenType* type) {
    this->type = type;
    return;
}

void Token::setValue(std::string value) {
    this->value = value;
    return;
}
