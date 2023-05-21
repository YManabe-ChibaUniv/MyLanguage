#include "instruction.h"

Instruction::Instruction(OpCode opCode, std::string operand) {
    this->opCode = opCode;
    this->operand = operand;
}

Instruction::~Instruction() {

}

OpCode Instruction::getOpCode() {
    return this->opCode;
}

std::string Instruction::getOperand() {
    return this->operand;
}
