#include "call_stack.h"

CallStack::CallStack(void) {
    this->stack_pointer = -1;
}

CallStack::~CallStack() {

}

void CallStack::push(int value) {
    this->stack_pointer++;
    this->stack[this->stack_pointer] = value;
}

int CallStack::pop(void) {
    this->stack_pointer--;
    return this->stack[this->stack_pointer + 1];
}

int CallStack::top(void) {
    return this->stack[this->stack_pointer];
}

int CallStack::size(void) {
    return this->stack_pointer + 1;
}
