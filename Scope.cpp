//
// Created by Nick Chapman on 11/23/16.
//

#include "Scope.h"

std::shared_ptr<type_union> Scope::getVariable(std::string& variableName) {
    if (symbolTable.find(variableName) != symbolTable.end()) {
        return this->symbolTable.at(variableName);
    }
    else {
        return nullptr;
    }
}

Scope::Scope() {
    this->symbolTable = std::unordered_map<std::string, std::shared_ptr<type_union>>();
}
