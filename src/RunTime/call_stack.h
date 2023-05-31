#ifndef _CALL_STACK_
#define _CALL_STACK_

#include "var.h"
#include "../config.h"

class CallStack {
    private:
        int stack[STACK_SIZE];
        int stack_pointer;
    public:
        CallStack(void);
        ~CallStack();
        void push(int value);
        int pop(void);
        int top(void);
        int size(void);
};

#endif
