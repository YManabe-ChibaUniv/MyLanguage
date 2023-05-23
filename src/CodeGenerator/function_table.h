#ifndef _FUNCTION_TABLE_
#define _FUNCTION_TABLE_

#include <iostream>
#include <vector>
#include <string>

class FunctionTable {
    public:
        FunctionTable();
        ~FunctionTable();

        void add(std::string name, int address);
        std::string getName(int index);
        int getAddress(std::string name);
        int getIndex(std::string name);
        void printTable(void);
    private:
        std::vector<std::pair<std::string, int>> names;
};

#endif
