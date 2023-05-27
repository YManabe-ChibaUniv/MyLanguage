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
    PRINT,
    RETURN,
    JUMP,
    HALT,
    LABEL,
    CALL,
};

#endif
