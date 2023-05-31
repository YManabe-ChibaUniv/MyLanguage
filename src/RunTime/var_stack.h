#ifndef _VAR_STACK_
#define _VAR_STACK_

#include "var.h"
#include "../config.h"

class VarStack {
    private:
        Var stack[STACK_SIZE] = {Var(), Var(), Var(), Var(), Var(), Var(), Var(), Var(), Var(), Var(), Var(), Var(), Var(), Var(), Var(), Var(), Var(), Var(), Var(), Var()};
        int stack_pointer;
    public:
        VarStack(void);
        ~VarStack();
        void push(int value);
        void push(std::string value);
        Var pop(void);
        Var top(void);
        int size(void);
};

#endif
