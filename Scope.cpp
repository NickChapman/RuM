//
// Created by Nick Chapman on 11/23/16.
//

#include <iostream>
#include "Scope.h"

Scope::Scope(std::shared_ptr<Scope> parentScope) {
    this->parentScope = parentScope;
    this->symbolTable = std::unordered_map<std::string, std::shared_ptr<TypeStruct>>();
    this->functionTable = std::unordered_map<std::string, std::shared_ptr<Function>>();
}

std::shared_ptr<TypeStruct> Scope::getVariable(std::string &variableName) {
    Scope *currentScope = this;
    while(currentScope != nullptr) {
        if (currentScope->symbolTable.find(variableName) != currentScope->symbolTable.end()) {
            return currentScope->symbolTable.at(variableName);
        }
        currentScope = currentScope->getParentScope().get();
    }
    return nullptr;
}

void Scope::setVariable(std::string variableName, std::shared_ptr<TypeStruct> value) {
    auto iterator = this->symbolTable.find(variableName);
    if (iterator != this->symbolTable.end()) {
        iterator->second = value;
    }
    else {
        this->symbolTable.emplace(variableName, value);
    }
}

std::shared_ptr<Function> Scope::getFunction(std::string functionName) {
    Scope *currentScope = this;
    while (currentScope != nullptr) {
        if (currentScope->functionTable.find(functionName) != currentScope->functionTable.end()) {
            return currentScope->functionTable.at(functionName);
        }
        currentScope = currentScope->getParentScope().get();
    }
    return nullptr;

}

void Scope::setFunction(std::string functionName, std::shared_ptr<Function> function) {
    auto iterator = this->functionTable.find(functionName);
    if (iterator != this->functionTable.end()) {
        iterator->second = function;
    }
    else {
        this->functionTable.emplace(functionName, function);
    }
}

std::shared_ptr<Scope> Scope::getParentScope() {
    return this->parentScope;
}
