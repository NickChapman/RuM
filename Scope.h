//
// Created by Nick Chapman on 11/23/16.
//

#ifndef RUM_SCOPE_H
#define RUM_SCOPE_H

#include <unordered_map>
#include <cstdlib>
#include "Type.h"

struct TypeStruct {
    union type_union {
        Type<int> *intType;
        Type<float> *floatType;
        Type<std::string> *stringType;
        Type<bool> *boolType;

        type_union() {
            intType = nullptr;
        }

        ~type_union() {
            //std::free(intType);
        }
    };

    type_union typeUnion;
    char activeType;

    TypeStruct() {
        typeUnion = type_union();
        activeType = 'N';
    }

    TypeStruct(TypeStruct *other) {
        this->typeUnion = other->typeUnion;
        this->activeType = other->activeType;
    }

    ~TypeStruct() {}
};

class Scope {

    std::unordered_map<std::string, std::shared_ptr<TypeStruct>> symbolTable;

public:
    std::shared_ptr<TypeStruct> getVariable(std::string &variableName);

    void setVariable(std::string &variableName, std::shared_ptr<TypeStruct>); // TODO

    Scope();
};


#endif //RUM_SCOPE_H
