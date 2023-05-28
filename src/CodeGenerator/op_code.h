#ifndef _OP_CODE_
#define _OP_CODE_

#include <stdint.h>

enum class OpCode : uint8_t {
    PUSH_INT,
    PUSH_STRING,
    LOAD_INT,
    LOAD_STRING,
    STORE_INT,
    STORE_STRING,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    AND,
    OR,
    NOT,
    CMP_EQUAL,
    CMP_NOT_EQUAL,
    CMP_LESS,
    CMP_LESS_EQUAL,
    CMP_GREATER,
    CMP_GREATER_EQUAL,
    PRINT,
    RETURN,
    JUMP,
    HALT,
    LABEL,
    CALL,
    JUMP_IF_FALSE,
};

#endif
