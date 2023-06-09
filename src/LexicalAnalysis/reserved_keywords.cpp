#include "reserved_keywords.h"

ReservedKeywords::ReservedKeywords(void) {
    this->setTokenKeywords();
    this->setPunctuators();
    this->setOperators();
    this->keywords_length = this->keywords.size();
    this->punctuators_length = this->punctuators.size();
    this->operators_length = this->operators.size();
}

int ReservedKeywords::getKeywordsLength(void) {
    return this->keywords_length;
}

std::map<TokenDetail, std::string> ReservedKeywords::getTokenKeywords(void) {
    return this->keywords;
}

int ReservedKeywords::getPunctuatorsLength(void) {
    return this->punctuators_length;
}

std::map<TokenDetail, std::string> ReservedKeywords::getPunctuators(void) {
    return this->punctuators;
}

int ReservedKeywords::getOperatorsLength(void) {
    return this->operators_length;
}

std::map<TokenDetail, std::string> ReservedKeywords::getOperators(void) {
    return this->operators;
}

void ReservedKeywords::setTokenKeywords(void) {
    this->keywords.emplace(TokenDetail::DEF_FUNCTION, "function");
    this->keywords.emplace(TokenDetail::DEF_INT32, "int32");
    this->keywords.emplace(TokenDetail::DEF_FLOAT32, "float32");
    this->keywords.emplace(TokenDetail::DEF_STRING, "string");
    this->keywords.emplace(TokenDetail::DEF_VOID, "void");
    this->keywords.emplace(TokenDetail::DEF_RETURN, "return");
    this->keywords.emplace(TokenDetail::IF, "if");
    this->keywords.emplace(TokenDetail::ELSE, "else");
    this->keywords.emplace(TokenDetail::TRUE, "TRUE");
    this->keywords.emplace(TokenDetail::FALSE, "FALSE");
    this->keywords.emplace(TokenDetail::WHILE, "while");
    this->keywords.emplace(TokenDetail::BREAK, "break");
    this->keywords.emplace(TokenDetail::CONTINUE, "continue");
    return;
}

void ReservedKeywords::setPunctuators(void) {
    this->punctuators.emplace(TokenDetail::LEFT_PARENTHESE, "(");
    this->punctuators.emplace(TokenDetail::RIGHT_PARENTHESE, ")");
    this->punctuators.emplace(TokenDetail::LEFT_BRACKET, "{");
    this->punctuators.emplace(TokenDetail::RIGHT_BRACKET, "}");
    this->punctuators.emplace(TokenDetail::SEMICOLON, ";");
    this->punctuators.emplace(TokenDetail::COMMA, ",");
    this->punctuators.emplace(TokenDetail::DOUBLE_QUOTATION, "\"");
    return;
}

void ReservedKeywords::setOperators(void) {
    this->operators.emplace(TokenDetail::RETURN_TYPE, "->");
    this->operators.emplace(TokenDetail::MOD, "MOD");
    this->operators.emplace(TokenDetail::AND, "AND");
    this->operators.emplace(TokenDetail::OR, "OR");
    this->operators.emplace(TokenDetail::NOT, "NOT");
    this->operators.emplace(TokenDetail::CMP_EQUAL, "==");
    this->operators.emplace(TokenDetail::CMP_NOT_EQUAL, "!=");
    this->operators.emplace(TokenDetail::SMALLER_EQUAL, "<=");
    this->operators.emplace(TokenDetail::LARGER_EQUAL, ">=");
    this->operators.emplace(TokenDetail::EQUAL, "=");
    this->operators.emplace(TokenDetail::NOT_SYM, "!");
    this->operators.emplace(TokenDetail::PLUS, "+");
    this->operators.emplace(TokenDetail::MINUS, "-");
    this->operators.emplace(TokenDetail::TIMES, "*");
    this->operators.emplace(TokenDetail::DIVISION, "/");
    this->operators.emplace(TokenDetail::LEFT_ANGLE_BRACKET, "<");
    this->operators.emplace(TokenDetail::RIGHT_ANGLE_BRACKET, ">");
    return;
}
