#pragma once
#ifndef _RESERVED_KEYWORDS_
#define _RESERVED_KEYWORDS_
#include "token_enum.h"

#include <iostream>
#include <map>

#define MAX_TOKEN_LEN 1024

class ReservedKeywords {
    protected:
        int keywords_length;
        std::map<TokenDetail, std::string> keywords;
        int punctuators_length;
        std::map<TokenDetail, std::string> punctuators;
        int operators_length;
        std::map<TokenDetail, std::string> operators;

    public:
        ReservedKeywords(void);
        int getKeywordsLength(void);
        std::map<TokenDetail, std::string> getTokenKeywords(void);
        int getPunctuatorsLength(void);
        std::map<TokenDetail, std::string> getPunctuators(void);
        int getOperatorsLength(void);
        std::map<TokenDetail, std::string> getOperators(void);

    private:
        void setTokenKeywords(void);
        void setPunctuators(void);
        void setOperators(void);
};

#endif
