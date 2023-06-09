#include "var.h"

Var::Var(void) {

}

Var::Var(int intValue) {
    this->intValue = intValue;
    this->type = 1;
}

Var::Var(float floatValue) {
    this->floatValue = floatValue;
    this->type = 3;
}

Var::Var(std::string stringValue) {
    this->stringValue = stringValue;
    this->type = 2;
}

Var::~Var() {

}

int Var::getIntValue(void) {
    return this->intValue;
}

float Var::getFloatValue(void) {
    return this->floatValue;
}

std::string Var::getStringValue(void) {
    return this->stringValue;
}

void Var::setIntValue(int intValue) {
    this->intValue = intValue;
    this->type = 1;
}

void Var::setFloatValue(float floatValue) {
    this->floatValue = floatValue;
    this->type = 3;
}

void Var::setStringValue(std::string stringValue) {
    this->stringValue = stringValue;
    this->type = 2;
}

int Var::getType(void) {
    return this->type;
}

std::string Var::__str(void) {
    if (this->type == 1) {
        return std::to_string(this->intValue);
    }
    else if (this->type == 2) {
        return this->stringValue;
    }
    else if (this->type == 3) {
        return std::to_string(this->floatValue);
    }

    return "";
}
