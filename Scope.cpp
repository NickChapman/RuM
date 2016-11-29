//
// Created by Nick Chapman on 11/23/16.
//

#include <iostream>
#include "Scope.h"

std::shared_ptr<TypeStruct> Scope::getVariable(std::string& variableName) {
    if (symbolTable.find(variableName) != symbolTable.end()) {
        return this->symbolTable.at(variableName);
    }
    else {
        return nullptr;
    }
}

Scope::Scope() {
    this->symbolTable = std::unordered_map<std::string, std::shared_ptr<TypeStruct>>();
}

void Scope::setVariable(std::string variableName, std::shared_ptr<TypeStruct> value) {
    auto iterator = this->symbolTable.find(variableName);
    if(iterator != this->symbolTable.end()) {
        iterator->second = value;
    }
    else {
        this->symbolTable.emplace(variableName, value);
    }
}
