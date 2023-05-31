#ifndef _SETUP_
#define _SETUP_

#define ENABLE_LEXICAL_ANALYSIS true
#define ENABLE_PARSER true
#define ENABLE_CODE_GENERATION true
#define ENABLE_RUNTIME true

#define RUNTIME_FILE_SIZE 1000000
#define STACK_SIZE 20
#define CALL_STACK_SIZE 20

#define INPUT_SOURCE_FILE "source.m"
#define OUTPUT_DUMP_FILE "dump/code.txt"
#define OUTPUT_RUNTIME_FILE "dump/runtime"
#define OUTPUT_RUNTIME_LOG_FILE "dump/runtime_log.txt"
#define OUTPUT_DISP_LOG_FILE "dump/disp_log.txt"
#define OUTPUT_OPCODE_FILE "dump/op_code.txt"

#endif
