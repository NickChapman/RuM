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

Scope::Scope(std::shared_ptr<Scope> parentScope) {
    this->parentScope = parentScope;
    this->symbolTable = std::unordered_map<std::string, std::shared_ptr<TypeStruct>>();
    this->functionTable = std::unordered_map<std::string, std::shared_ptr<Function>>();
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

std::shared_ptr<Function> Scope::getFunction(std::string functionName) {
    if (this->functionTable.find(functionName) != this->functionTable.end()) {
        return this->functionTable.at(functionName);
    }
    else {
        return nullptr;
    }
}

void Scope::setFunction(std::string functionName, std::shared_ptr<Function> function) {
    auto iterator = this->functionTable.find(functionName);
    if(iterator != this->functionTable.end()) {
        iterator->second = function;
    }
    else {
        this->functionTable.emplace(functionName, function);
    }
}

std::shared_ptr<Scope> Scope::getParentScope() {
    return this->parentScope;
}
