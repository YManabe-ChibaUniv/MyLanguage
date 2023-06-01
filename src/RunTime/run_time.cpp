#include "run_time.h"

RunTime::RunTime(std::string runtime_file_name) {
    this->runtime_file.open(runtime_file_name, std::ios::in | std::ios::binary);
    if (!runtime_file.is_open()) {
        std::cout << "Error: RunTime::RunTime: runtime_file_name: " << runtime_file_name << std::endl;
        throw;
    }

    #if DEBUG
        this->runtime_log_file = std::ofstream(OUTPUT_RUNTIME_LOG_FILE, std::ios::out | std::ios::trunc);
        this->disp_log_file = std::ofstream(OUTPUT_DISP_LOG_FILE, std::ios::out | std::ios::trunc);
    #endif

    this->runtime_file.seekg(0, std::ios::end);
    std::streampos runtime_file_size = runtime_file.tellg();
    this->runtime_file.seekg(0, std::ios::beg);

    this->runtime_file.read(this->runtime, runtime_file_size);
    this->runtime_iterator = 0;
    this->runtime_begin = 0;
    this->runtime_end = this->runtime_file.gcount();
}

RunTime::~RunTime() {
    this->runtime_file.close();
    #if DEBUG
        this->runtime_log_file.close();
        this->disp_log_file.close();
    #endif

    this->vars.clear();
}

void RunTime::run(void) {
    uint8_t op_code;
    uint8_t str_size;
    int int32_value;
    float float32_value;
    std::string str_value;
    int var_address;
    int function_address;
    Var var1;
    Var var2;

    while (this->runtime_iterator < this->runtime_end) {
        op_code = this->runtime[this->runtime_iterator];
        this->runtime_iterator++;

        switch (static_cast<OpCode>(op_code)) {
            case OpCode::PUSH_INT:
                // int32 value
                int32_value = this->readIntValueByIterator();
                this->stack.push(int32_value);
                #if DEBUG
                    this->runtime_log_file << "PUSH_INT: " << int32_value << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::PUSH_FLOAT:
                // float value
                float32_value = this->readFloatValueByIterator();
                this->stack.push(float32_value);
                #if DEBUG
                    this->runtime_log_file << "PUSH_FLOAT: " << float32_value << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::PUSH_STRING:
                // str size
                str_size = this->readIntValueByIterator();
                // str value
                str_value = this->readStringValueByIterator(str_size);
                this->stack.push(str_value);
                #if DEBUG
                    this->runtime_log_file << "PUSH_STRING: " << str_value << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::LOAD_INT:
                // var index (int)
                var_address = this->readIntValueByIterator();
                this->stack.push(this->vars[var_address].getIntValue());
                #if DEBUG
                    this->runtime_log_file << "LOAD_INT: " << var_address << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::LOAD_FLOAT:
                // var index (int)
                var_address = this->readIntValueByIterator();
                this->stack.push(this->vars[var_address].getFloatValue());
                #if DEBUG
                    this->runtime_log_file << "LOAD_FLOAT: " << var_address << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::LOAD_STRING:
                // var index (int)
                var_address = this->readIntValueByIterator();
                this->stack.push(this->vars[var_address].getStringValue());
                #if DEBUG
                    this->runtime_log_file << "LOAD_STRING: " << var_address << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::STORE_INT:
                // var index (int)
                var_address = this->readIntValueByIterator();
                this->vars[var_address] = this->stack.pop();
                #if DEBUG
                    this->runtime_log_file << "STORE_INT: " << var_address << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::STORE_FLOAT:
                // var index (int)
                var_address = this->readIntValueByIterator();
                this->vars[var_address] = this->stack.pop();
                #if DEBUG
                    this->runtime_log_file << "STORE_FLOAT: " << var_address << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::STORE_STRING:
                // var index (int)
                var_address = this->readIntValueByIterator();
                this->vars[var_address] = this->stack.pop();
                #if DEBUG
                    this->runtime_log_file << "STORE_STRING: " << var_address << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::ADD:
                var1 = this->stack.pop();
                var2 = this->stack.pop();

                // int32 + int32
                if (var2.getType() == 1) {
                    #if DEBUG
                        this->runtime_log_file << "int32 + int32" << std::endl;
                    #endif
                    this->stack.push(var2.getIntValue() + var1.getIntValue());
                }
                // string + string
                else if (var2.getType() == 2) {
                    #if DEBUG
                        this->runtime_log_file << "string + string" << std::endl;
                    #endif
                    this->stack.push(var2.getStringValue() + var1.getStringValue());
                }
                // float32 + float32
                else if (var2.getType() == 3) {
                    #if DEBUG
                        this->runtime_log_file << "float32 + float32" << std::endl;
                    #endif
                    this->stack.push(var2.getFloatValue() + var1.getFloatValue());
                }
                #if DEBUG
                    this->runtime_log_file << "ADD: " << this->stack.top().getIntValue() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::SUB:
                var1 = this->stack.pop();
                var2 = this->stack.pop();
                // int32 - int32
                if (var2.getType() == 1) {
                    this->stack.push(var2.getIntValue() - var1.getIntValue());
                }
                // float32 - float32
                else if (var2.getType() == 3) {
                    this->stack.push(var2.getFloatValue() - var1.getFloatValue());
                }
                #if DEBUG
                    this->runtime_log_file << "SUB: " << this->stack.top().getIntValue() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::MUL:
                var1 = this->stack.pop();
                var2 = this->stack.pop();
                // int32 * int32
                if (var2.getType() == 1) {
                    this->stack.push(var2.getIntValue() * var1.getIntValue());
                }
                // float32 * float32
                else if (var2.getType() == 3) {
                    this->stack.push(var2.getFloatValue() * var1.getFloatValue());
                }
                #if DEBUG
                    this->runtime_log_file << "MUL: " << this->stack.top().getIntValue() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::DIV:
                var1 = this->stack.pop();
                var2 = this->stack.pop();
                // int32 / int32
                if (var2.getType() == 1) {
                    this->stack.push(var2.getIntValue() / var1.getIntValue());
                }
                // float32 / float32
                else if (var2.getType() == 3) {
                    this->stack.push(var2.getFloatValue() / var1.getFloatValue());
                }
                #if DEBUG
                    this->runtime_log_file << "DIV: " << this->stack.top().getIntValue() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::MOD:
                var1 = this->stack.pop();
                var2 = this->stack.pop();
                if (var2.getType() == 1) {
                    this->stack.push(var2.getIntValue() % var1.getIntValue());
                }
                #if DEBUG
                    this->runtime_log_file << "MOD: " << this->stack.top().getIntValue() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::AND:
                var1 = this->stack.pop();
                var2 = this->stack.pop();
                if (var1.getIntValue() == 0 || var2.getIntValue() == 0) {
                    this->stack.push(0);
                }
                else {
                    this->stack.push(1);
                }
                #if DEBUG
                    this->runtime_log_file << "AND: " << var2.__str() << " && " << var1.__str() << " " << this->stack.top().getIntValue() << std::endl;
                    this->logStackAndMap();
                #endif

                break;
            case OpCode::OR:
                var1 = this->stack.pop();
                var2 = this->stack.pop();
                if (var1.getIntValue() != 0 || var2.getIntValue() != 0) {
                    this->stack.push(1);
                }
                else {
                    this->stack.push(0);
                }
                #if DEBUG
                    this->runtime_log_file << "AND: " << var2.__str() << " || " << var1.__str() << " " << this->stack.top().getIntValue() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::NOT:
                var1 = this->stack.pop();
                if (var1.getIntValue() == 0) {
                    this->stack.push(1);
                }
                else {
                    this->stack.push(0);
                }
                #if DEBUG
                    this->runtime_log_file << "NOT: " << var1.__str() << " " << this->stack.top().getIntValue() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::CMP_EQUAL:
                var1 = this->stack.pop();
                var2 = this->stack.pop();
                // int32 == int32
                if (var2.getType() == 1) {
                    #if DEBUG
                        this->runtime_log_file << "int32 + int32" << std::endl;
                    #endif
                    if (var2.getIntValue() == var1.getIntValue()) {
                        this->stack.push(1);
                    }
                    else {
                        this->stack.push(0);
                    }
                }
                // string == string
                else if (var2.getType() == 2) {
                    #if DEBUG
                        this->runtime_log_file << "string + string" << std::endl;
                    #endif
                    if (var2.getStringValue() == var1.getStringValue()) {
                        this->stack.push(1);
                    }
                    else {
                        this->stack.push(0);
                    }
                }
                // float32 == float32
                else if (var2.getType() == 3) {
                    #if DEBUG
                        this->runtime_log_file << "float32 + float32" << std::endl;
                    #endif
                    if (var2.getFloatValue() == var1.getFloatValue()) {
                        this->stack.push(1);
                    }
                    else {
                        this->stack.push(0);
                    }
                }
                #if DEBUG
                    this->runtime_log_file << "CMP_EQUAL: " << var2.__str() << " == " << var1.__str() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::CMP_NOT_EQUAL:
                var1 = this->stack.pop();
                var2 = this->stack.pop();
                // int32 != int32
                if (var2.getType() == 1) {
                    #if DEBUG
                        this->runtime_log_file << "int32 + int32" << std::endl;
                    #endif
                    if (var2.getIntValue() == var1.getIntValue()) {
                        this->stack.push(0);
                    }
                    else {
                        this->stack.push(1);
                    }
                }
                // string != string
                else if (var2.getType() == 2) {
                    #if DEBUG
                        this->runtime_log_file << "string + string" << std::endl;
                    #endif
                    if (var2.getStringValue() == var1.getStringValue()) {
                        this->stack.push(0);
                    }
                    else {
                        this->stack.push(1);
                    }
                }
                // float32 != float32
                else if (var2.getType() == 3) {
                    #if DEBUG
                        this->runtime_log_file << "float32 + float32" << std::endl;
                    #endif
                    if (var2.getFloatValue() == var1.getFloatValue()) {
                        this->stack.push(0);
                    }
                    else {
                        this->stack.push(1);
                    }
                }
                #if DEBUG
                    this->runtime_log_file << "CMP_NOT_EQUAL: " << var2.__str() << " != " << var1.__str() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::CMP_LESS:
                var1 = this->stack.pop();
                var2 = this->stack.pop();
                // int32 < int32
                if (var2.getType() == 1) {
                    if (var2.getIntValue() < var1.getIntValue()) {
                    this->stack.push(1);
                }
                else {
                    this->stack.push(0);
                }
                }
                // float32 < float32
                else if (var2.getType() == 3) {
                    if (var2.getFloatValue() < var1.getFloatValue()) {
                    this->stack.push(1);
                }
                else {
                    this->stack.push(0);
                }
                }
                #if DEBUG
                    this->runtime_log_file << "CMP_LESS: " << var2.__str() << " < " << var1.__str() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::CMP_LESS_EQUAL:
                var1 = this->stack.pop();
                var2 = this->stack.pop();
                // int32 <= int32
                if (var2.getType() == 1) {
                    if (var2.getIntValue() <= var1.getIntValue()) {
                        this->stack.push(1);
                    }
                    else {
                        this->stack.push(0);
                    }
                }
                // float32 <= float32
                else if (var2.getType() == 3) {
                    if (var2.getFloatValue() <= var1.getFloatValue()) {
                        this->stack.push(1);
                    }
                    else {
                        this->stack.push(0);
                    }
                }
                #if DEBUG
                    this->runtime_log_file << "CMP_LESS_EQUAL: " << var2.__str() << " <= " << var1.__str() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::CMP_GREATER:
                var1 = this->stack.pop();
                var2 = this->stack.pop();
                // int32 > int32
                if (var2.getType() == 1) {
                    if (var2.getIntValue() > var1.getIntValue()) {
                        this->stack.push(1);
                    }
                    else {
                        this->stack.push(0);
                    }
                }
                // float32 > float32
                else if (var2.getType() == 3) {
                    if (var2.getFloatValue() > var1.getFloatValue()) {
                        this->stack.push(1);
                    }
                    else {
                        this->stack.push(0);
                    }
                }
                #if DEBUG
                    this->runtime_log_file << "CMP_GREATER: " << var2.__str() << " > " << var1.__str() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::CMP_GREATER_EQUAL:
                var1 = this->stack.pop();
                var2 = this->stack.pop();
                // int32 >= int32
                if (var2.getType() == 1) {
                    if (var2.getIntValue() >= var1.getIntValue()) {
                        this->stack.push(1);
                    }
                    else {
                        this->stack.push(0);
                    }
                }
                // float32 >= float32
                else if (var2.getType() == 3) {
                    if (var2.getFloatValue() >= var1.getFloatValue()) {
                        this->stack.push(1);
                    }
                    else {
                        this->stack.push(0);
                    }
                }
                #if DEBUG
                    this->runtime_log_file << "CMP_GREATER_EQUAL: " << var2.__str() << " >= " << var1.__str() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::PRINT:
                var1 = this->stack.pop();
                std::cout << var1.__str();

                #if DEBUG
                    this->disp_log_file << var1.__str();
                    this->runtime_log_file << "PRINT" << std::endl;
                    this->logStackAndMap();
                #endif

                break;
            case OpCode::RETURN:
                // back to call function address
                this->runtime_iterator = this->call_stack.pop();
                #if DEBUG
                    this->runtime_log_file << "RETURN" << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::JUMP:
                // function address
                function_address = this->readIntValueByIterator();
                this->runtime_iterator = function_address;
                #if DEBUG
                    this->runtime_log_file << "JUMP: " << function_address << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::HALT:
                #if DEBUG
                    this->runtime_log_file << "HALT" << std::endl;
                    this->logStackAndMap();
                #endif
                return;
                break;
            case OpCode::LABEL:
                // function address
                function_address = this->readIntValueByIterator();
                // TODO: add label to map

                #if DEBUG
                    this->runtime_log_file << "LABEL: " << function_address << std::endl;
                    this->logStackAndMap();
                #else
                    var_address = function_address;
                #endif
                break;
            case OpCode::CALL:
                function_address = this->readIntValueByIterator();
                this->call_stack.push(this->runtime_iterator);
                this->runtime_iterator = function_address;

                #if DEBUG
                    this->runtime_log_file << "CALL: " << function_address << std::endl;
                    this->logStackAndMap();
                #endif

                break;
            case OpCode::JUMP_IF_FALSE:
                var1 = this->stack.pop();
                // function address
                function_address = this->readIntValueByIterator();
                if (var1.getIntValue() == 0) {
                    this->runtime_iterator = this->runtime_begin + function_address;
                }
                #if DEBUG
                    this->runtime_log_file << "JUMP_IF_FALSE: " << function_address << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            default:
                return;
        }
    }

    return;
}

int RunTime::readIntValueByIterator(void) {
    int int_value = 0x00000000;
    int filter = 0x000000FF;

    for (int i = 0; i < INT32_SIZE; i++) {
        int_value |= filter & (this->runtime[this->runtime_iterator] << (i * 8));
        filter <<= 8;
        this->runtime_iterator++;
    }

    return int_value;
}

float RunTime::readFloatValueByIterator(void) {
    float float_value;
    std::memcpy(&float_value, &this->runtime[this->runtime_iterator], FLOAT32_SIZE);
    this->runtime_iterator += FLOAT32_SIZE;

    return float_value;
}

std::string RunTime::readStringValueByIterator(int size) {
    std::string str_value = "";

    for (int i = 0; i < size; i++) {
        str_value += this->runtime[this->runtime_iterator];
        this->runtime_iterator++;
    }

    return str_value;
}

void RunTime::logStackAndMap(void) {
    this->runtime_log_file << "\"\"\"" << std::endl;
    this->runtime_log_file << "Stack:" << std::endl;
    VarStack stack_copy = this->stack;
    int i = 0;
    while (stack_copy.size() > 0) {
        this->runtime_log_file << i++ << ": " << stack_copy.pop().__str() << std::endl;
    }
    this->runtime_log_file << std::endl;

    this->runtime_log_file << "Map:" << std::endl;
    for (auto& var : this->vars) {
        this->runtime_log_file << var.first << ": " << var.second.__str() << std::endl;
    }
    this->runtime_log_file << std::endl;

    this->runtime_log_file << "Call stack:" << std::endl;
    CallStack call_stack_copy = this->call_stack;
    i = 0;
    while (call_stack_copy.size() > 0) {
        this->runtime_log_file << i++ << ": " << call_stack_copy.pop() << std::endl;
    }
    this->runtime_log_file << "\"\"\"" << std::endl;

    return;
}
