#ifndef _TOKEN_TYPE_
#define _TOKEN_TYPE_

#include "token_enum.h"

class TokenType {
    protected:
        TokenKind kind;
        TokenDetail detail;
    public:
        TokenType(TokenKind kind, TokenDetail detail);
        TokenKind getTokenKind(void);
        TokenDetail getTokenDetail(void);
};
#endif
