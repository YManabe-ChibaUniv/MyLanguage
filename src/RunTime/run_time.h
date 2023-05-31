#ifndef _RUN_TIME_
#define _RUN_TIME_

#include "../CodeGenerator/op_code.h"
#include "var.h"
#include "../debug.h"
#include "../config.h"
#include "var_stack.h"
#include "call_stack.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
#include <stack>
#include <map>

class RunTime {
    public:
        RunTime(std::string runtime_file_name);
        ~RunTime();
        void run(void);
    private:
        std::ifstream runtime_file;
        std::ofstream runtime_log_file;
        #if DEBUG
            std::ofstream disp_log_file;
        #endif
        char runtime[RUNTIME_FILE_SIZE];
        int runtime_iterator;
        int runtime_begin;
        int runtime_end;
        VarStack stack;
        std::map<int, Var> vars;
        CallStack call_stack;
        int readIntValueByIterator(void);
        std::string readStringValueByIterator(int size);
        void logStackAndMap(void);
};

#endif
