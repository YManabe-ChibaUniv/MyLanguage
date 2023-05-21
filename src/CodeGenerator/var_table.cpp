#include "var_table.h"

VarTable::VarTable() {

}

VarTable::~VarTable() {
    this->names.clear();
}

void VarTable::add(std::string name, TokenDetail type) {
    this->names.push_back(std::make_pair(name, type));
}

std::string VarTable::getName(int index) {
    return this->names[index].first;
}

TokenDetail VarTable::getType(int index) {
    return this->names[index].second;
}

int VarTable::getAddress(std::string name) {
    for (int i = 0; i < (int) this->names.size(); i++) {
        if (this->names[i].first == name) {
            return i;
        }
    }
    return -1;
}

void VarTable::printTable(void) {
    std::cout << "Variable Table:" << std::endl;
    for (int i = 0; i < (int) this->names.size(); i++) {
        std::cout << i << " " << this->names[i].first << " : ";
        switch (this->names[i].second) {
            case TokenDetail::DEF_INT32:
                std::cout << "int32";
                break;
            case TokenDetail::DEF_STRING:
                std::cout << "string";
                break;
            default:
                std::cout << "unknown";
                break;
        }
        std::cout << std::endl;
    }

    return;
}
