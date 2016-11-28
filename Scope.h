//
// Created by Nick Chapman on 11/23/16.
//

#ifndef RUM_SCOPE_H
#define RUM_SCOPE_H

#include <unordered_map>
#include "Type.h"

union type_union {
    std::shared_ptr<Type<int>> intType;
    std::shared_ptr<Type<float>> floatType;
    std::shared_ptr<Type<std::string>> stringType;
    std::shared_ptr<Type<bool>> boolType;

    type_union() {
        intType = nullptr;
        floatType = nullptr;
        stringType = nullptr;
        boolType = nullptr;
    }

    type_union(type_union *other) {
        this->intType = other->intType;
        this->floatType = other->floatType;
        this->stringType = other->stringType;
        this->boolType = other->boolType;
    }

    ~type_union() {}
};

class Scope {

    std::unordered_map<std::string, std::shared_ptr<type_union>> symbolTable;

public:
    std::shared_ptr<type_union> getVariable(std::string &variableName);

    void setVariable(std::string &variableName, std::shared_ptr<type_union>) {

    }

    Scope();
};


#endif //RUM_SCOPE_H
