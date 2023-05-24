#ifndef _RUN_TIME_
#define _RUN_TIME_

#include "../CodeGenerator/op_code.h"
#include "var.h"
#include "../debug.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
#include <stack>
#include <map>

class RunTime {
    public:
        RunTime(std::string runtime_file_name, std::string runtime_log_file_name);
        ~RunTime();
        void run(void);
    private:
        std::ifstream runtime_file;
        std::ofstream runtime_log_file;
        std::vector<char> runtime;
        std::vector<char>::iterator runtime_iterator;
        std::vector<char>::iterator runtime_begin;
        std::vector<char>::iterator runtime_end;
        std::stack<Var*> stack;
        std::map<int, Var*> vars;
        std::stack<int> call_stack;
        int readIntValueByIterator(void);
        std::string readStringValueByIterator(int size);
        void logStackAndMap(void);
};

#endif
