//
// Created by Nick Chapman on 11/23/16.
//

#ifndef RUM_SCOPE_H
#define RUM_SCOPE_H

#include <unordered_map>
#include "Type.h"

class Scope {
    std::unordered_map<std::string, std::shared_ptr<TypeStruct>> symbolTable;

public:
    std::shared_ptr<TypeStruct> getVariable(std::string &variableName);

    void setVariable(std::string &variableName, std::shared_ptr<TypeStruct>); // TODO

    Scope();
};


#endif //RUM_SCOPE_H
