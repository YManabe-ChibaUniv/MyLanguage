#include "function_table.h"

FunctionTable::FunctionTable() {

}

FunctionTable::~FunctionTable() {
    this->names.clear();
}

void FunctionTable::add(std::string name, int address) {
    this->names.push_back(std::make_pair(name, address));
}

std::string FunctionTable::getName(int index) {
    return this->names[index].first;
}

int FunctionTable::getAddress(std::string name) {
    for (int i = 0; i < (int) this->names.size(); i++) {
        if (this->names[i].first == name) {
            return this->names[i].second;
        }
    }
    return -1;
}

void FunctionTable::printTable(void) {
    std::cout << "Function Table" << std::endl;
    for (int i = 0; i < (int) this->names.size(); i++) {
        std::cout << i << ": " << this->names[i].first << ", " << this->names[i].second << std::endl;
    }
}
