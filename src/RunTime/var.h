#ifndef _VAR_
#define _VAR_

#include <string>
#include <iostream>

class Var {
    private:
        int intValue;
        std::string stringValue;
        int type;
    public:
        Var(void);
        Var(int intValue);
        Var(std::string stringValue);
        ~Var();
        int getIntValue(void);
        std::string getStringValue(void);
        void setIntValue(int intValue);
        void setStringValue(std::string stringValue);
        int getType(void);
        std::string __str(void);
};

#endif
