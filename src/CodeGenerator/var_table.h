#ifndef _VAR_TABLE_
#define _VAR_TABLE_

#include "../LexicalAnalysis/token_enum.h"

#include <vector>
#include <string>
#include <iostream>

class VarTable {
    public:
        VarTable();
        ~VarTable();

        void add(std::string name, TokenDetail type);
        std::string getName(int index);
        TokenDetail getType(int index);
        int getAddress(std::string name);
        void printTable(void);
    private:
        std::vector<std::pair<std::string, TokenDetail>> names;
};

#endif
