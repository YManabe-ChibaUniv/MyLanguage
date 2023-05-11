#ifndef _TOKEN_STRUCTURE_
#define _TOKEN_STRUCTURE_

#include "token_enum.h"

// Token Type Structure
typedef struct TokenType TokenType;
struct TokenType {
    TokenKind kind;
    TokenDetail detail;
};

// Token Structure
typedef struct Token Token;
struct Token {
    TokenType type;  // Token type
    Token *next;     // Next token
    int val;         // Numeric literal value
    char *str;       // Token string
    int len;         // Token length
};

typedef struct TokenKeyWord TokenKeyWord;
struct TokenKeyWord {
    TokenDetail token_detail;
    char* keyword;
};

#endif
