#ifndef _LEXICAL_ANALYSIS_
#define _LEXICAL_ANALYSIS_

#define KEYWORD_LIST_FILE "dump/keywords.txt"
#define TOKEN_LIST_FILE "dump/tokens.txt"

#include "reserved_keywords.h"
#include "token.h"
#include "../debug.h"

#include <fstream>
#include <vector>

class LexicalAnalysis {
    protected:
        std::ifstream ifs;
        ReservedKeywords* rk;
    public:
        LexicalAnalysis(const std::string fileName);
        std::vector<Token*> run(void);
        ~LexicalAnalysis();
    private:
        char nextChar(void);
        void unget(void);
        void runError(std::string str);
        bool isOperator(const char c);
        bool isPunctuation(const char c);
};

#endif
