#include "run_time.h"

#define INT32_SIZE 4
#define STR_SIZE 4
#define LONG_SIZE 8

RunTime::RunTime(std::string runtime_file_name, std::string runtime_log_file_name) {
    this->runtime_file.open(runtime_file_name, std::ios::in | std::ios::binary);
    if (!runtime_file.is_open()) {
        std::cout << "Error: RunTime::RunTime: runtime_file_name: " << runtime_file_name << std::endl;
        throw;
    }

    this->runtime_log_file = std::ofstream(runtime_log_file_name, std::ios::out | std::ios::trunc);

    this->runtime_file.seekg(0, std::ios::end);
    std::streampos runtime_file_size = runtime_file.tellg();
    this->runtime_file.seekg(0, std::ios::beg);

    this->runtime.resize(runtime_file_size);
    this->runtime_file.read(this->runtime.data(), runtime_file_size);
    this->runtime_iterator = this->runtime.begin();
    this->runtime_begin = this->runtime.begin();
    this->runtime_end = this->runtime.end();
}

RunTime::~RunTime() {
    this->runtime_file.close();
    this->runtime_log_file.close();

    this->stack.empty();
    this->vars.clear();
    this->call_stack.empty();
}

void RunTime::run(void) {
    uint8_t op_code;
    uint8_t str_size;
    int int32_value;
    std::string str_value;
    int var_address;
    int function_address;
    Var* var1;
    Var* var2;

    while (this->runtime_iterator != this->runtime_end) {
        op_code = *this->runtime_iterator;
        this->runtime_iterator++;

        switch (static_cast<OpCode>(op_code)) {
            case OpCode::PUSH_INT:
                // int32 value
                int32_value = this->readIntValueByIterator();
                this->stack.push(new Var(int32_value));
                #if DEBUG
                    this->runtime_log_file << "PUSH_INT: " << int32_value << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::PUSH_STRING:
                // str size
                str_size = this->readIntValueByIterator();
                // str value
                str_value = this->readStringValueByIterator(str_size);
                this->stack.push(new Var(str_value));
                #if DEBUG
                    this->runtime_log_file << "PUSH_STRING: " << str_value << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::LOAD_INT:
                // var index (int)
                var_address = this->readIntValueByIterator();
                this->stack.push(new Var(this->vars[var_address]->getIntValue()));
                #if DEBUG
                    this->runtime_log_file << "LOAD_INT: " << var_address << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::LOAD_STRING:
                // var index (int)
                var_address = this->readIntValueByIterator();
                this->stack.push(new Var(this->vars[var_address]->getStringValue()));
                #if DEBUG
                    this->runtime_log_file << "LOAD_STRING: " << var_address << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::STORE_INT:
                // var index (int)
                var_address = this->readIntValueByIterator();
                this->vars[var_address] = this->stack.top();
                this->stack.pop();
                #if DEBUG
                    this->runtime_log_file << "STORE_INT: " << var_address << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::STORE_STRING:
                // var index (int)
                var_address = this->readIntValueByIterator();
                this->vars[var_address] = this->stack.top();
                this->stack.pop();
                #if DEBUG
                    this->runtime_log_file << "STORE_STRING: " << var_address << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::ADD:
                var1 = this->stack.top();
                this->stack.pop();
                var2 = this->stack.top();
                this->stack.pop();

                // int32 + int32
                if (var2->getType() == 1) {
                    #if DEBUG
                        this->runtime_log_file << "int32 + int32" << std::endl;
                    #endif
                    this->stack.push(new Var(var2->getIntValue() + var1->getIntValue()));
                }
                // string + string
                else if (var2->getType() == 2) {
                    #if DEBUG
                        this->runtime_log_file << "string + string" << std::endl;
                    #endif
                    this->stack.push(new Var(var2->getStringValue() + var1->getStringValue()));
                }
                delete var1;
                delete var2;
                #if DEBUG
                    this->runtime_log_file << "ADD: " << this->stack.top()->getIntValue() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::SUB:
                var1 = this->stack.top();
                this->stack.pop();
                var2 = this->stack.top();
                this->stack.pop();
                this->stack.push(new Var(var2->getIntValue() - var1->getIntValue()));
                delete var1;
                delete var2;
                #if DEBUG
                    this->runtime_log_file << "SUB: " << this->stack.top()->getIntValue() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::MUL:
                var1 = this->stack.top();
                this->stack.pop();
                var2 = this->stack.top();
                this->stack.pop();
                this->stack.push(new Var(var2->getIntValue() * var1->getIntValue()));
                delete var1;
                delete var2;
                #if DEBUG
                    this->runtime_log_file << "MUL: " << this->stack.top()->getIntValue() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::DIV:
                var1 = this->stack.top();
                this->stack.pop();
                var2 = this->stack.top();
                this->stack.pop();
                this->stack.push(new Var(var2->getIntValue() / var1->getIntValue()));
                delete var1;
                delete var2;
                #if DEBUG
                    this->runtime_log_file << "DIV: " << this->stack.top()->getIntValue() << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::PRINT:
                var1 = this->stack.top();
                this->stack.pop();
                std::cout << var1->__str();
                delete var1;
                #if DEBUG
                    this->runtime_log_file << "PRINT" << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::RETURN:
                // back to call function address
                this->runtime_iterator = this->runtime_begin + this->call_stack.top();
                this->call_stack.pop();
                #if DEBUG
                    this->runtime_log_file << "RETURN" << std::endl;
                    this->logStackAndMap();
                #endif
                break;
            case OpCode::JUMP:
                // function address
                function_address = this->readIntValueByIterator();
                this->runtime_iterator = this->runtime_begin + function_address;
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
                this->call_stack.push(std::distance(this->runtime_begin, this->runtime_iterator));
                this->runtime_iterator = this->runtime_begin + function_address;

                #if DEBUG
                    this->runtime_log_file << "CALL: " << function_address << std::endl;
                    this->logStackAndMap();
                #endif

                break;
        }
    }

    return;
}

int RunTime::readIntValueByIterator(void) {
    int int_value = 0;

    for (int i = 0; i < INT32_SIZE; i++) {
        int_value |= (*this->runtime_iterator << (i * 8));
        this->runtime_iterator++;
    }

    return int_value;
}

std::string RunTime::readStringValueByIterator(int size) {
    std::string str_value = "";

    for (int i = 0; i < size; i++) {
        str_value += *this->runtime_iterator;
        this->runtime_iterator++;
    }

    return str_value;
}

void RunTime::logStackAndMap(void) {
    this->runtime_log_file << "\"\"\"" << std::endl;
    this->runtime_log_file << "Stack:" << std::endl;
    std::stack<Var*> stack_copy = this->stack;
    int i = 0;
    while (!stack_copy.empty()) {
        this->runtime_log_file << i++ << ": " << stack_copy.top()->__str() << std::endl;
        stack_copy.pop();
    }
    this->runtime_log_file << std::endl;

    this->runtime_log_file << "Map:" << std::endl;
    for (auto& var : this->vars) {
        this->runtime_log_file << var.first << ": " << var.second->__str() << std::endl;
    }
    this->runtime_log_file << std::endl;

    this->runtime_log_file << "Call stack:" << std::endl;
    std::stack<int> call_stack_copy = this->call_stack;
    i = 0;
    while (!call_stack_copy.empty()) {
        this->runtime_log_file << i++ << ": " << call_stack_copy.top() << std::endl;
        call_stack_copy.pop();
    }
    this->runtime_log_file << "\"\"\"" << std::endl;

    return;
}
