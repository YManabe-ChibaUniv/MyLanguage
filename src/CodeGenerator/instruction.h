#ifndef _INSTRUCTION_
#define _INSTRUCTION_

#include "op_code.h"

#include <string>

class Instruction {
    public:
        Instruction(OpCode opCode, std::string operand);
        ~Instruction();

        OpCode getOpCode();
        std::string getOperand();
    private:
        OpCode opCode;
        std::string operand;
};

#endif
