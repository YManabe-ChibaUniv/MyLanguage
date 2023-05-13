#include "lexical_analysis.h"

LexicalAnalysis::LexicalAnalysis(const std::string fileName) {
    this->ifs.open(fileName);
    this->rk = new ReservedKeywords();

    #if DEBUG
        std::ofstream ofs(KEYWORD_LIST_FILE);
        ofs << "keywords num: " << this->rk->getKeywordsLength() << std::endl;
        for (std::pair<TokenDetail, std::string> p : this->rk->getTokenKeywords()) {
            ofs << static_cast<int>(p.first) << " => " << p.second << std::endl;
        }
        ofs << "punctuators num: " << this->rk->getPunctuatorsLength() << std::endl;
        for (std::pair<TokenDetail, std::string> p : this->rk->getPunctuators()) {
            ofs << static_cast<int>(p.first) << " => " << p.second << std::endl;
        }
        ofs << "operators num: " << this->rk->getOperatorsLength() << std::endl;
        for (std::pair<TokenDetail, std::string> p : this->rk->getOperators()) {
            ofs << static_cast<int>(p.first) << " => " << p.second << std::endl;
        }
        ofs.close();
    #endif
}

LexicalAnalysis::~LexicalAnalysis() {
    this->ifs.close();
    delete(this->rk);
}

std::vector<Token*> LexicalAnalysis::run(void) {
    std::vector<Token*> tokens;
    Token* token = NULL;
    char c = this->nextChar();

    // analyze
    while(c != EOF) {
        if (token == NULL) {
            token = new Token();
        }
        // skip space
        while (std::isspace(c)) {
            c = this->nextChar();
        }

        // Check for identifiers and keywords
        if (std::isalpha(c)) {
            std::string identifier;
            while (std::isalpha(c) || std::isdigit(c)) {
                identifier += c;
                c = this->nextChar();
            }
            this->unget();

            bool isKeyword = false;
            token->setValue(identifier);
            for (std::pair<TokenDetail, std::string> p : this->rk->getTokenKeywords()) {
                if (identifier == p.second) {
                    isKeyword = true;
                    token->setTokenType(new TokenType(TokenKind::TK_KEYWORD, p.first));
                    break;
                }
            }
            if (!isKeyword) {
                token->setTokenType(new TokenType(TokenKind::TK_IDENT, TokenDetail::NOTHING));
            }
        }

        // Check for numeric literals
        else if (std::isdigit(c)) {
            std::string number;
            while (std::isdigit(c)) {
                number += c;
                c = this->nextChar();
            }
            this->unget();
            token->setTokenType(new TokenType(TokenKind::TK_NUM, TokenDetail::NOTHING));
            token->setValue(number);
        }

        // Check for string literals
        else if (c == '\"') {
            std::string str;
            c = this->nextChar();
            while (c != '\"') {
                if (c == EOF) {
                    this->runError("Reaching EOF before \"");
                }
                str += c;
                c = this->nextChar();
            }
            token->setTokenType(new TokenType(TokenKind::TK_STRING, TokenDetail::NOTHING));
            token->setValue(str);
        }

        // Check for comment
        else if (c == '/') {
            c = this->nextChar();
            if (c == '*') {
                while (true) {
                    c = this->nextChar();
                    if (c == EOF) {
                        this->runError("Reaching EOF before closing comment");
                    }
                    else if (c == '*') {
                        c = this->nextChar();
                        if (c == '/') {
                            break;
                        }
                        this->unget();
                    }
                }
                c = this->nextChar();
                continue;
            }
            else {
                this->unget();
                this->unget();
                c = this->nextChar();
            }
        }

        // Check for operators and punctuation
        else {
            // Check for operators
            if (this->isOperator(c)) {
                std::string op;
                while (this->isOperator(c)) {
                    op += c;
                    c = this->nextChar();
                }
                bool isOperator = false;
                for (std::pair<TokenDetail, std::string> p : this->rk->getOperators()) {
                    if (op == p.second) {
                        token->setTokenType(new TokenType(TokenKind::TK_OPERATOR, p.first));
                        token->setValue(op);
                        isOperator = true;
                        break;
                    }
                }
                if (!isOperator) {
                    std::string err = "Can't Understand this Operator ";
                    err += op;
                    this->runError(err);
                }
            }
            // Check for punctuation
            else if (this->isPunctuation(c)) {
                for (std::pair<TokenDetail, std::string> p : this->rk->getPunctuators()) {
                    std::string pun(1, c);
                    if (pun == p.second) {
                        token->setTokenType(new TokenType(TokenKind::TK_PUNCTUATOR, p.first));
                        token->setValue(pun);
                        break;
                    }
                }
            }
            else {
                if (c == EOF) {
                    break;
                }
                std::string errMsg("Can't Understand token: ");
                errMsg += c;
                errMsg += " [";
                errMsg += std::to_string(c);
                errMsg += "]";
                this->runError(errMsg);
            }
        }
        if (token->getTokenType() != NULL) {
            tokens.push_back(token);
            token = NULL;
            c = this->nextChar();
        }
        else {
            this->runError("Throw Exception");
        }
    }

    if (token != NULL) {
        delete(token);
    }

    #if DEBUG
        std::ofstream ofs(TOKEN_LIST_FILE);
        ofs << "token num: " << tokens.size() << std::endl;
        for (Token* token : tokens) {
            TokenType* tt = token->getTokenType();
            ofs << "[" << static_cast<int>(tt->getTokenKind()) << ", " << static_cast<int>(tt->getTokenDetail()) << "] " << token->getValue() << std::endl;
        }
        ofs.close();
    #endif

    return tokens;
}

char LexicalAnalysis::nextChar(void) {
    return this->ifs.get();
}

void LexicalAnalysis::unget(void) {
    this->ifs.unget();
    return;
}

void LexicalAnalysis::runError(std::string str) {
    std::cerr << str << std::endl;
    throw str;
}

bool LexicalAnalysis::isOperator(const char c) {
    std::string op = "";
    op += c;
    for (std::pair<TokenDetail, std::string> p : this->rk->getOperators()) {
        if (op == p.second) {
            return true;
        }
    }
    return false;
}

bool LexicalAnalysis::isPunctuation(const char c) {
    std::string pun = "";
    pun += c;
    for (std::pair<TokenDetail, std::string> p : this->rk->getPunctuators()) {
        if (pun == p.second) {
            return true;
        }
    }
    return false;
}
