//
// Created by Nick Chapman on 11/23/16.
//

#ifndef RUM_SCOPE_H
#define RUM_SCOPE_H

#include <unordered_map>
#include "Type.h"
#include "Token.h"
#include "Function.h"

class Scope {
    std::shared_ptr<Scope> parentScope = nullptr;

    std::unordered_map<std::string, std::shared_ptr<TypeStruct>> symbolTable;

    std::unordered_map<std::string, std::shared_ptr<Function>> functionTable;
public:
    Scope(std::shared_ptr<Scope> parentScope);

    std::shared_ptr<Scope> getParentScope();

    std::shared_ptr<TypeStruct> getVariable(std::string &variableName);

    void setVariable(std::string variableName, std::shared_ptr<TypeStruct> value);

    void setVariable(std::string variableName, std::shared_ptr<TypeStruct> value, bool deep);

    std::shared_ptr<Function> getFunction(std::string functionName);

    void setFunction(std::string functionName, std::shared_ptr<Function> function);
};

#endif //RUM_SCOPE_H
