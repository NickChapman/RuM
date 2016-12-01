//
// Created by Nick Chapman on 11/30/16.
//

#ifndef RUM_FUNCTION_H
#define RUM_FUNCTION_H

#include <vector>
#include "Token.h"

class Function {
    std::string returnVariable;
    std::shared_ptr<std::vector<std::string>> argumentsToBind;
    std::shared_ptr<std::vector<Token>> instructions;

public:
    Function(std::string returnVariable, std::shared_ptr<std::vector<std::string>> argumentsToBind,
             std::shared_ptr<std::vector<Token>> instructions) {
        this->returnVariable = returnVariable;
        this->argumentsToBind = argumentsToBind;
        this->instructions = instructions;
    }

    std::string getReturnVariable() {
        return this->returnVariable;
    }

    std::shared_ptr<std::vector<std::string>> getArgumentsToBind() {
        return this->argumentsToBind;
    }

    std::shared_ptr<std::vector<Token>> getInstructions(){
        return this->instructions;
    }
};

#endif //RUM_FUNCTION_H
