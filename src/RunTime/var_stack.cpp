#include "var_stack.h"

VarStack::VarStack(void) {
    this->stack_pointer = -1;
}

VarStack::~VarStack() {

}

void VarStack::push(int value) {
    this->stack_pointer++;
    this->stack[this->stack_pointer].setIntValue(value);
}

void VarStack::push(std::string value) {
    this->stack_pointer++;
    this->stack[this->stack_pointer].setStringValue(value);
}

Var VarStack::pop(void) {
    this->stack_pointer--;
    return this->stack[this->stack_pointer + 1];
}

Var VarStack::top(void) {
    return this->stack[this->stack_pointer];
}

int VarStack::size(void) {
    return this->stack_pointer + 1;
}
