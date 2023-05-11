#ifndef _TOKEN_
#define _TOKEN_

#include "token_type.h"

#include <string>

class Token {
    protected:
        TokenType* type;
        std::string value;
    public:
        Token(void);
        ~Token();
        TokenType* getTokenType(void);
        std::string getValue(void);
        void setTokenType(TokenType* type);
        void setValue(std::string value);
};

#endif
