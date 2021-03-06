//
// Created by Nick Chapman on 10/27/16.
//

#include<iostream>
#include <bootstrap.h>
#include "RuMParser.h"
#include "BooleanOperations.h"

// We don't actually do anything in this case
// We need a token list in order to do anything
RuMParser::RuMParser() {}

RuMParser::RuMParser(std::shared_ptr<std::vector<Token>> tokenList) {
    this->tokenList = tokenList;
    this->tokenListPosition = 0;
    this->parseTreeOutputBuffer = "";
    this->lastExpression = "";

    this->globalScope = std::make_shared<Scope>(nullptr);

    // Build our keyword parsing map
    keywordParseMap["if_key"] = "[keyword if]";
    keywordParseMap["else_key"] = "[keyword else]";
    keywordParseMap["endif_key"] = "[keyword endif]";
    keywordParseMap["while_key"] = "[keyword while]";
    keywordParseMap["endwhile_key"] = "[keyword endwhile]";
    keywordParseMap["new_key"] = "[keyword new]";
    keywordParseMap["classdef_key"] = "[keyword classdef]";
    keywordParseMap["inherits_key"] = "[keyword inherits]";
    keywordParseMap["from_key"] = "[keyword from]";
    keywordParseMap["endclass_key"] = "[keyword endclass]";
    keywordParseMap["function_key"] = "[keyword fundef]";
    keywordParseMap["endfunction_key"] = "[keyword endfun]";
    keywordParseMap["true_key"] = "[keyword TRUE]";
    keywordParseMap["false_key"] = "[keyword FALSE]";
    keywordParseMap["null_key"] = "[keyword NULL]";
    keywordParseMap["exit_token"] = "[keyword exit]";

    // Build our operator parsing map
    operatorParseMap["negative_op"] = "[operator '-']";
    operatorParseMap["plus_op"] = "[operator '+']";
    operatorParseMap["mult_op"] = "[operator '*']";
    operatorParseMap["div_op"] = "[operator '/']";
    operatorParseMap["exponent_op"] = "[operator '^']";
    operatorParseMap["assignment_op"] = "[operator '=']";
    operatorParseMap["bool_equals"] = "[operator '==']";
    operatorParseMap["bool_lessthan"] = "[operator '<']";
    operatorParseMap["bool_greaterthan"] = "[operator '>']";
    operatorParseMap["bool_and"] = "[operator '&&']";
    operatorParseMap["bool_or"] = "[operator '||']";
    operatorParseMap["bool_not"] = "[operator '!']";
    operatorParseMap["reference_op"] = "[operator '&']";
    operatorParseMap["open_paren"] = "[operator '(']";
    operatorParseMap["close_paren"] = "[operator ')']";
    operatorParseMap["left_square_bracket"] = "[operator '[']";
    operatorParseMap["right_square_bracket"] = "[operator ']']";
    operatorParseMap["comma"] = "[operator ',']";
    operatorParseMap["dot_op"] = "[operator '.']";
    operatorParseMap["semicolon"] = "[operator ';']";
}

void RuMParser::setTokenList(std::shared_ptr<std::vector<Token>> tokenList) {
    this->tokenList = tokenList;
    this->tokenListPosition = 0;
}

void RuMParser::setDisplayParseTree(bool display) {
    this->displayParseTree = display;
}

bool RuMParser::getDisplayParseTree() const {
    return this->displayParseTree;
}

// Helper function
std::string RuMParser::currentTokenType() {
    return tokenList->at(tokenListPosition).getTokenType();
}

void RuMParser::setLastExpression(std::shared_ptr<TypeStruct> &expression) {
    switch (expression->activeType) {
        case 'I':
            this->lastExpression = std::to_string(expression->typeUnion.intType->getValue());
            break;
        case 'F':
            this->lastExpression = std::to_string(expression->typeUnion.floatType->getValue());
            break;
        case 'S':
            this->lastExpression = "'" + expression->typeUnion.stringType->getValue() + "'";
            break;
        case 'B':
            if (expression->typeUnion.boolType->getValue()) {
                this->lastExpression = "TRUE";
            }
            else {
                this->lastExpression = "FALSE";
            }
            break;
        case 'N':
            this->lastExpression = "nullptr";
            break;
        default:
            this->lastExpression = "nullptr";
    }
}

std::shared_ptr<std::vector<Token>>
RuMParser::copyTokenSection(std::shared_ptr<std::vector<Token>> target, unsigned long startPosition,
                            unsigned long endPosition) {
    std::shared_ptr<std::vector<Token>> ret = std::make_shared<std::vector<Token>>();
    for (unsigned long i = startPosition; i <= endPosition; ++i) {
        ret->push_back(target->at(i));
    }
    return ret;
}

void RuMParser::reset() {
    this->parseTreeOutputBuffer = "";
    this->tokenListPosition = 0;
}

void RuMParser::parseProgram() {
    // Set up the scopes
    this->currentScope = globalScope;
    // Parse the program
    parseTreeOutputBuffer += "[PROGRAM ";
    parseStmtList();
    parseTreeOutputBuffer += "]";
    if (this->displayParseTree) {
        std::cout << "CONSTRUCTED PARSE TREE:" << std::endl;
        std::cout << parseTreeOutputBuffer << std::endl;
    }
    std::cout << lastExpression << std::endl;
    reset();
}

void RuMParser::parseStmtList() {
    parseTreeOutputBuffer += "[STMT-LIST ";
    while (currentTokenType() != "else_key" && currentTokenType() != "endif_key" &&
           currentTokenType() != "endwhile_key" &&
           currentTokenType() != "endfunction_key" &&
           currentTokenType() != "endinput") {
        parseStmt();
    }
    parseTreeOutputBuffer += "]";
}

void RuMParser::parseStmt() {
    //<ARG> | <IF_STMT> | <WHILE> | <FUNC> | <CLASS> | <ASSIGN>
    parseTreeOutputBuffer += "[STMT ";
    if (currentTokenType() == "if_key") {
        parseIfStmt();
    }
    else if (currentTokenType() == "while_key") {
        parseWhile();
    }
    else if (currentTokenType() == "function_key") {
        parseFunc();
    }
    else if (currentTokenType() == "classdef_key") {
        parseClass();
    }
    else if (currentTokenType() == "identifier" &&
             (tokenListPosition + 1 < tokenList->size()) &&
             tokenList->at(tokenListPosition + 1).getTokenType() == "assignment_op") {
        parseAssign();
    }
    else {
        parseArg();
    }
    parseTreeOutputBuffer += "]";
}

std::string RuMParser::parseAssign() {
    // TODO: Make assignments shallow copies
    parseTreeOutputBuffer += "[ASSIGN ";
    if (currentTokenType() == "identifier") {
        std::string identifier = parseVar();
        if (currentTokenType() == "assignment_op") {
            parseOperator();
            std::shared_ptr<TypeStruct> value;
            // Here we do what's necessary to perform shallow copying
            bool deep = false;
            if (currentTokenType() == "identifier" &&
                tokenList->at(tokenListPosition + 1).getTokenType() == "semicolon") {
                // We need to look up the RHS variable
                std::string rhsVariable = parseVar();
                value = this->currentScope->getVariable(rhsVariable);
                if (value == nullptr) {
                    throw std::runtime_error("Reference to undefined variable " + rhsVariable);
                }
            }
            else {
                // Scan to determine whether this is going to be a shallow or deep copy
                // If the RHS has another variable in it then we're going to deep copy
                unsigned long startingPosition = tokenListPosition;
                while (currentTokenType() != "semicolon" and tokenListPosition < tokenList->size() - 1) {
                    if (currentTokenType() == "identifier" &&
                        tokenList->at(tokenListPosition + 1).getTokenType() != "dot_op" &&
                        tokenList->at(tokenListPosition + 1).getTokenType() != "open_paren") {
                        deep = true;
                    }
                    ++tokenListPosition;
                }
                tokenListPosition = startingPosition;
                value = parseArg();
            }
            if (currentTokenType() == "semicolon") {
                parseOperator();
                // Save the variable
                if (deep) {
                    this->currentScope->setVariable(identifier, value, deep);
                }
                else {
                    this->currentScope->setVariable(identifier, value);
                }
            }
            else {
                throw std::runtime_error("Expected ';' but instead received '" + currentTokenType() + "'.");
            }
        }
        else {
            throw std::runtime_error("Expected '=' but instead received '" + currentTokenType() + "'.");
        }
    }
    else {
        throw std::runtime_error("Expected 'identifier' but instead received '" + currentTokenType() + "'.");
    }
    parseTreeOutputBuffer += "]";
    return ""; // TODO
}

void RuMParser::parseFunc() {
    std::string functionName;
    std::string returnVariable;
    std::shared_ptr<std::vector<std::string>> argumentsToBind = std::make_shared<std::vector<std::string>>();
    std::shared_ptr<std::vector<Token>> instructions;
    parseTreeOutputBuffer += "[FUNC ";
    if (currentTokenType() == "function_key") {
        parseKeyword();
        if (currentTokenType() == "identifier") {
            returnVariable = parseVar();
            if (currentTokenType() == "assignment_op") {
                parseOperator();
                if (currentTokenType() == "identifier") {
                    functionName = parseIdentifier();
                    if (currentTokenType() == "open_paren") {
                        parseOperator();
                        // Parse a list of vars
                        while (currentTokenType() == "identifier") {
                            argumentsToBind->push_back(parseVar());
                            if (currentTokenType() == "comma") {
                                parseOperator();
                                // If we just parsed a comma we need to make sure there is an identifier coming
                                if (currentTokenType() != "identifier") {
                                    throw std::runtime_error(
                                            "Expected 'identifier' but instead received '" + currentTokenType() + "'.");
                                }
                            }
                        }
                        if (currentTokenType() == "close_paren") {
                            parseOperator();
                            unsigned long startPosition = tokenListPosition;
                            // Now we need to gather the instructions for this function
                            while (currentTokenType() != "endfunction_key") {
                                ++tokenListPosition;
                            }
                            unsigned long endPosition = tokenListPosition;
                            instructions = this->copyTokenSection(this->tokenList, startPosition, endPosition);
                            std::shared_ptr<Function> newFunction = std::make_shared<Function>(returnVariable,
                                                                                               argumentsToBind,
                                                                                               instructions);
                            // One final check to make sure everything is right
                            if (currentTokenType() == "endfunction_key") {
                                parseKeyword();
                            }
                            else {
                                throw std::runtime_error(
                                        "Expected 'endfun' but instead received '" + currentTokenType() + "'.");
                            }
                            // Finally save it
                            this->currentScope->setFunction(functionName, newFunction);
                        }
                        else {
                            throw std::runtime_error("Expected ')' but instead received '" + currentTokenType() + "'.");
                        }
                    }
                    else {
                        throw std::runtime_error("Expected '(' but instead received '" + currentTokenType() + "'.");
                    }
                }
                else {
                    throw std::runtime_error(
                            "Expected 'identifier' but instead received '" + currentTokenType() + "'.");
                }
            }
            else {
                throw std::runtime_error("Expected '=' but instead received '" + currentTokenType() + "'.");
            }
        }
        else {
            throw std::runtime_error("Expected 'variable' but instead received '" + currentTokenType() + "'.");
        }
    }
    else {
        throw std::runtime_error("Expected 'fundef' but instead received '" + currentTokenType() + "'.");
    }
    parseTreeOutputBuffer += "]";
}

std::shared_ptr<TypeStruct> RuMParser::parseInvoke() {
    std::string functionName;
    std::shared_ptr<Function> function;
    std::shared_ptr<std::vector<std::shared_ptr<TypeStruct>>> arguments;
    parseTreeOutputBuffer += "[INVOKE ";
    if (currentTokenType() == "identifier") {
        functionName = parseIdentifier();
        if (functionName == "print") {
            printFunction();
            return nullptr;
        }
        if (functionName == "copy") {
            return std::make_shared<TypeStruct>(parseExpr());
        }
        if (currentTokenType() == "open_paren") {
            parseOperator();
            arguments = parseArgList();
            if (currentTokenType() == "close_paren") {
                parseOperator();
            }
            else {
                throw std::runtime_error("Expected ')' but instead received '" + currentTokenType() + "'.");
            }
            // Now we evaluate the function
            function = this->currentScope->getFunction(functionName);
            if (function == nullptr) {
                throw std::runtime_error("Function `" + functionName + "` is not defined.");
            }
            if (arguments->size() != function->getArgumentsToBind()->size()) {
                throw std::runtime_error("Function `" + functionName + "` takes " +
                                         std::to_string(function->getArgumentsToBind()->size()) +
                                         " arguments but " +
                                         std::to_string(arguments->size()) + " were given.");
            }
            // Assuming everything is good so far
            std::shared_ptr<Scope> callingScope = this->currentScope;
            std::shared_ptr<Scope> functionScope = std::make_shared<Scope>(this->globalScope);
            std::shared_ptr<std::vector<Token>> mainTokenList = this->tokenList;
            unsigned long mainTokenListPosition = this->tokenListPosition;
            try {
                this->currentScope = functionScope;
                // Bind all of the arguments to variables in the function scope
                for (unsigned long i = 0; i < arguments->size(); ++i) {
                    this->currentScope->setVariable(function->getArgumentsToBind()->at(i), arguments->at(i));
                }
                // Now run the code
                this->tokenList = function->getInstructions();
                this->tokenListPosition = 0;
                parseStmtList();
                // Get the return value
                std::string returnVariable = function->getReturnVariable();
                std::shared_ptr<TypeStruct> ret = this->currentScope->getVariable(returnVariable);
                this->currentScope = callingScope;
                this->tokenList = mainTokenList;
                this->tokenListPosition = mainTokenListPosition;
                return ret;
            }
            catch (const std::runtime_error &e) {
                // Clean up and restore the scope before propagating the error
                this->currentScope = callingScope;
                this->tokenList = mainTokenList;
                this->tokenListPosition = mainTokenListPosition;
                throw e;
            }

        }
        else {
            throw std::runtime_error("Expected '(' but instead received '" + currentTokenType() + "'.");
        }
    }
    else {
        throw std::runtime_error("Expected 'identifier' but instead received '" + currentTokenType() + "'.");
    }
    parseTreeOutputBuffer += "]";
    return nullptr; // TODO
}

std::shared_ptr<std::vector<std::shared_ptr<TypeStruct>>> RuMParser::parseArgList() {
    parseTreeOutputBuffer += "[ARG-LIST ";
    std::shared_ptr<std::vector<std::shared_ptr<TypeStruct>>> values = std::make_shared<std::vector<std::shared_ptr<TypeStruct>>>();
    // Need to make sure there are actually arguments to parse
    if (currentTokenType() != "close_paren") {
        values->push_back(parseArg());
        while (currentTokenType() == "comma") {
            parseOperator();
            values->push_back(parseArg());
        }
    }
    parseTreeOutputBuffer += "]";
    return values;
}

std::shared_ptr<TypeStruct> RuMParser::parseArg() {
    parseTreeOutputBuffer += "[ARG ";
    std::shared_ptr<TypeStruct> ret;
    if (currentTokenType() == "reference_op") {
        parseRef();
    }
    else if (currentTokenType() == "left_square_bracket") {
        parseList();
    }
    else if (currentTokenType() == "new_key") {
        parseAnonClass();
    }
    else {
        ret = parseExpr();
    }
    parseTreeOutputBuffer += "]";
    setLastExpression(ret);
    return ret;
}

void RuMParser::parseRef() {
    parseTreeOutputBuffer += "[REF ";
    if (currentTokenType() == "reference_op") {
        parseOperator();
        parseVar();
    }
    else {
        throw std::runtime_error("Expected '&' but instead received '" + currentTokenType() + "'.");
    }
    parseTreeOutputBuffer += "]";
}

void RuMParser::parseAnonClass() {
    parseTreeOutputBuffer += "[ANON-CLASS";
    if (currentTokenType() == "new_key") {
        parseKeyword();
        if (currentTokenType() == "open_paren") {
            parseOperator();
            if (currentTokenType() == "string") {
                parseString();
                if (currentTokenType() == "close_paren") {
                    parseOperator();
                    parseTreeOutputBuffer += "]";
                }
                else {
                    throw std::runtime_error("Expected ')' but instead received '" + currentTokenType() + "'.");
                }
            }
            else {
                throw std::runtime_error("Expected 'string' but instead received '" + currentTokenType() + "'.");
            }
        }
        else {
            throw std::runtime_error("Expected '(' but instead received '" + currentTokenType() + "'.");
        }
    }
    else {
        throw std::runtime_error("Expected 'new' but instead received '" + currentTokenType() + "'.");
    }
}

void RuMParser::parseClass() {
    parseTreeOutputBuffer += "[CLASS ";
    if (currentTokenType() == "classdef_key") {
        parseKeyword();
        if (currentTokenType() == "identifier") {
            parseIdentifier();
            // Now we need to figure out if it inherits from anything
            if (currentTokenType() == "inherits_key") {
                parseKeyword();
                // There should now be a from keyword
                if (currentTokenType() == "from_key") {
                    parseKeyword();
                    parseIdentifier();
                }
                else {
                    throw std::runtime_error("Expected 'from' but instead received '" + currentTokenType() + "'.");
                }
            }
            // Now regardless we have the class block
            parseClassBlock();
            // Now we should be on the endclass keyword
            if (currentTokenType() == "endclass_key") {
                parseKeyword();
            }
            else {
                throw std::runtime_error("Expected 'endclass' but instead received '" + currentTokenType() + "'.");
            }
        }
        else {
            throw std::runtime_error("Expected 'identifier' but instead received '" + currentTokenType() + "'.");
        }
    }
    else {
        throw std::runtime_error("Expected 'classdef' but instead received '" + currentTokenType() + "'.");
    }
    parseTreeOutputBuffer += "]";
}

void RuMParser::parseClassBlock() {
    parseTreeOutputBuffer += "[CLASS-BLOCK ";
    while (currentTokenType() != "endclass_key") {
        // We are not done with the class items
        parseClassItem();
    }
    parseTreeOutputBuffer += "]";
}


void RuMParser::parseClassItem() {
    parseTreeOutputBuffer += "[CLASS-ITEM ";
    if (currentTokenType() == "function_key") {
        // It is a function definition
        parseFunc();
    }
    else {
        // Otherwise it has to be an assignment
        parseAssign();
    }
    parseTreeOutputBuffer += "]";
}

std::shared_ptr<TypeStruct> RuMParser::parseClassAccess() {
    // TODO
    parseTreeOutputBuffer += "[CLASS-ACCESS ";
    if (currentTokenType() == "identifier") {
        parseVar();
        parseClassAccessPrime();
    }
    else {
        throw std::runtime_error("Expected 'identifier' but instead recieved '" + currentTokenType() + "'.");
    }
    parseTreeOutputBuffer += "]";
    return nullptr;
}

void RuMParser::parseClassAccessPrime() {
    while (currentTokenType() == "dot_op") {
        parseTreeOutputBuffer += "[CLASS-ACCESS' ";
        parseOperator();
        if (tokenList->at(tokenListPosition + 1).getTokenType() == "open_paren") {
            // In this case it is a function invocation
            parseInvoke();
        }
        else {
            // Just a normal variable access
            parseVar();
        }
        parseTreeOutputBuffer += "]";
    }
}

void RuMParser::parseWhile() {
    std::shared_ptr<TypeStruct> condition;
    if (currentTokenType() == "while_key") {
        parseTreeOutputBuffer += "[WHILE ";
        parseKeyword();
        if (currentTokenType() == "open_paren") {
            parseOperator();
            unsigned long startOfCondition = tokenListPosition;
            condition = parseExpr();
            unsigned long endOfCondition = tokenListPosition;
            if (currentTokenType() == "close_paren") {
                parseOperator();
                unsigned long startPosition = tokenListPosition;
                unsigned long endPosition;
                if (BOOL::isTrue(condition)) {
                    auto mainTokenList = this->tokenList;
                    // We do it once to gather up the instruction set and then we repeat those instructions if it's still true
                    parseStmtList();
                    endPosition = tokenListPosition;
                    auto conditionTokens = copyTokenSection(this->tokenList, startOfCondition, endOfCondition);
                    this->setTokenList(conditionTokens);
                    this->tokenListPosition = 0;
                    condition = parseExpr();
                    if (BOOL::isTrue(condition)) {
                        auto instructions = copyTokenSection(mainTokenList, startPosition, endPosition);
                        auto temp_holder = this->tokenList;
                        this->setTokenList(instructions);
                        while (BOOL::isTrue(condition)) {
                            this->setTokenList(instructions);
                            this->tokenListPosition = 0;
                            parseStmtList();
                            this->setTokenList(conditionTokens);
                            this->tokenListPosition = 0;
                            condition = parseExpr();
                        }
                    }
                    this->setTokenList(mainTokenList);
                    this->tokenListPosition = endPosition;
                }
                else {
                    while (currentTokenType() != "endwhile_key") {
                        ++tokenListPosition;
                    }
                }
                if (currentTokenType() == "endwhile_key") {
                    parseKeyword();
                    parseTreeOutputBuffer += "]";
                }
                else {
                    throw std::runtime_error("Expected 'endwhile' but instead received '" + currentTokenType() + "'.");
                }
            }
            else {
                throw std::runtime_error("Expected ')' but instead received '" + currentTokenType() + "'.");
            }
        }
        else {
            throw std::runtime_error("Expected '(' but instead received '" + currentTokenType() + "'.");
        }
    }
    else {
        throw std::runtime_error("Expected 'while' but instead received '" + currentTokenType() + "'.");
    }
}

void RuMParser::parseIfStmt() {
    std::shared_ptr<TypeStruct> condition;
    if (currentTokenType() == "if_key") {
        parseTreeOutputBuffer += "[IF-STMT ";
        parseKeyword();
        if (currentTokenType() == "open_paren") {
            parseOperator();
            condition = parseExpr();
            // Check to make sure there is a closing paren
            if (currentTokenType() == "close_paren") {
                parseOperator();
                if (BOOL::isTrue(condition)) {
                    parseStmtList();
                    while (currentTokenType() != "endif_key" && tokenListPosition < tokenList->size()) {
                        ++tokenListPosition;
                    }
                }
                else {
                    while (currentTokenType() != "else_key" && currentTokenType() != "endif_key") {
                        ++tokenListPosition;
                    }
                    // Check to see if we finished on an else key
                    if (currentTokenType() == "else_key") {
                        parseKeyword();
                        parseStmtList();
                    }
                }
                // Now make sure that there is an endif
                if (currentTokenType() == "endif_key") {
                    parseKeyword();
                    parseTreeOutputBuffer += "]";
                }
                else {
                    throw std::runtime_error("Expected `endif_key` but instead received '" + currentTokenType() + "'.");
                }
            }
            else {
                throw std::runtime_error("Expected a ')' but instead received '" + currentTokenType() + "'.");
            }
        }
        else {
            throw std::runtime_error("Expected a '(' but instead received '" + currentTokenType() + "'.");
        }
    }
    else {
        throw std::runtime_error("Expected 'if' but instead received '" + currentTokenType() + "'.");
    }
}

std::shared_ptr<TypeStruct> RuMParser::parseExpr() {
    std::shared_ptr<TypeStruct> ret;
    parseTreeOutputBuffer += "[EXPR ";
    if (currentTokenType() == "bool_not") {
        parseOperator();
        ret = std::make_shared<TypeStruct>(parseExpr());
        BOOL::negate(ret);
    }
    else {
        ret = parseBool();
    }
    parseTreeOutputBuffer += "]";
    setLastExpression(ret);
    return ret;
}

std::shared_ptr<TypeStruct> RuMParser::parseBool() {
    parseTreeOutputBuffer += "[BOOL ";
    std::shared_ptr<TypeStruct> result = std::make_shared<TypeStruct>(parseBoolTerm());
    // Now we have to deal with boolean operations
    if (currentTokenType() == "bool_equals" ||
        currentTokenType() == "bool_lessthan" ||
        currentTokenType() == "bool_greaterthan" ||
        currentTokenType() == "bool_and" ||
        currentTokenType() == "bool_or") {
        // TODO: Test to make sure the result constructor produces a deep copy
        std::shared_ptr<TypeStruct> additional;
        while (currentTokenType() == "bool_equals" ||
               currentTokenType() == "bool_lessthan" ||
               currentTokenType() == "bool_greaterthan" ||
               currentTokenType() == "bool_and" ||
               currentTokenType() == "bool_or") {
            std::string operation = currentTokenType();
            parseOperator();
            // Handle each case
            if (operation == "bool_equals") {
                additional = parseBoolTerm();
                result->typeUnion.boolType->setValue(BOOL::equals(result, additional));
                result->activeType = 'B';
            }
            else if (operation == "bool_lessthan") {
                additional = parseBoolTerm();
                result->typeUnion.boolType->setValue(BOOL::lessThan(result, additional));
                result->activeType = 'B';
            }
            else if (operation == "bool_greaterthan") {
                additional = parseBoolTerm();
                result->typeUnion.boolType->setValue(BOOL::greaterThan(result, additional));
                result->activeType = 'B';
            }
            else if (operation == "bool_and") {
                additional = parseBoolTerm();
                result->typeUnion.boolType->setValue(BOOL::logicalAnd(result, additional));
                result->activeType = 'B';
            }
            else if (operation == "bool_or") {
                additional = parseBoolTerm();
                result->typeUnion.boolType->setValue(BOOL::logicalOr(result, additional));
                result->activeType = 'B';
            }
        }
    }
    parseTreeOutputBuffer += "]";
    return result;

}

std::shared_ptr<TypeStruct> RuMParser::parseBoolTerm() {
    // TODO: Add support for `NULL`
    parseTreeOutputBuffer += "[BOOL-TERM ";
    std::shared_ptr<TypeStruct> base = std::make_shared<TypeStruct>();
    if (currentTokenType() == "true_key") {
        parseKeyword();
        base->typeUnion.boolType = new Type<bool>("boolean", true);
        base->activeType = 'B';
    }
    else if (currentTokenType() == "false_key") {
        parseKeyword();
        base->typeUnion.boolType = new Type<bool>("boolean", false);
        base->activeType = 'B';
    }
    else {
        base = parseMathExpr();
    }
    parseTreeOutputBuffer += "]";
    return base;
}

std::shared_ptr<TypeStruct> RuMParser::parseMathExpr() {
    parseTreeOutputBuffer += "[MATH-EXPR ";
    // We handle strings at this level explicitly to handle using "+" for string concatenation
    std::shared_ptr<TypeStruct> ret;
    if (currentTokenType() == "string") {
        ret = parseString();
    }
    else {
        ret = std::make_shared<TypeStruct>(parseTerm());
    }
    while (currentTokenType() == "plus_op" || currentTokenType() == "negative_op") {
        std::string operation = currentTokenType();
        parseOperator();
        std::shared_ptr<TypeStruct> additional;
        if(currentTokenType() == "string") {
            additional = parseString();
        }
        else {
            additional = parseTerm();
            if(additional->activeType == 'N') {
                throw std::runtime_error("Expected a numeric type but received `nullptr`");
            }
        }
        // If it's a subtraction we'll perform addition on the negative
        if (operation == "negative_op") {
            if (additional->activeType == 'I') {
                additional->typeUnion.intType->setValue(additional->typeUnion.intType->getValue() * -1);
            }
            else {
                additional->typeUnion.floatType->setValue(additional->typeUnion.floatType->getValue() * -1);
            }
        }
        // Now do the addition
        // Check to make sure they're not using the bool type for addition because that's undefined
        if (additional->activeType == 'B') {
            throw std::runtime_error("boolean is an invalid type to use in addition or subtraction");
        }
        if (ret->activeType == 'I') {
            if (additional->activeType == 'I') {
                ret->typeUnion.intType->setValue(
                        ret->typeUnion.intType->getValue() + additional->typeUnion.intType->getValue());
            }
            else if (additional->activeType == 'F') {
                ret->typeUnion.floatType->setValue(
                        ret->typeUnion.intType->getValue() + additional->typeUnion.floatType->getValue());
                ret->activeType = 'F';
            }
            else {
                ret->typeUnion.stringType->setValue(std::to_string(ret->typeUnion.intType->getValue()) +
                                                    additional->typeUnion.stringType->getValue());
                ret->activeType = 'S';
            }
        }
        else if (ret->activeType == 'F') {
            if (additional->activeType == 'I') {
                ret->typeUnion.floatType->setValue(
                        ret->typeUnion.floatType->getValue() + additional->typeUnion.intType->getValue());
            }
            else if (additional->activeType == 'F') {
                ret->typeUnion.floatType->setValue(
                        ret->typeUnion.floatType->getValue() + additional->typeUnion.floatType->getValue());
            }
            else {
                ret->typeUnion.stringType->setValue(std::to_string(ret->typeUnion.floatType->getValue()) +
                                                    additional->typeUnion.stringType->getValue());
                ret->activeType = 'S';
            }
        }
        else {
            if (additional->activeType == 'I') {
                ret->typeUnion.stringType->setValue(ret->typeUnion.stringType->getValue() +
                                                    std::to_string(additional->typeUnion.intType->getValue()));
            }
            else if (additional->activeType == 'F') {
                ret->typeUnion.stringType->setValue(ret->typeUnion.stringType->getValue() +
                                                    std::to_string(additional->typeUnion.floatType->getValue()));
            }
            else {
                ret->typeUnion.stringType->setValue(
                        ret->typeUnion.stringType->getValue() + additional->typeUnion.stringType->getValue());
            }
        }
        parseTreeOutputBuffer += "]";
    }
    return ret;
}

std::shared_ptr<TypeStruct> RuMParser::parseTerm() {
    parseTreeOutputBuffer += "[TERM ";
    std::shared_ptr<TypeStruct> ret = std::make_shared<TypeStruct>(parseFactor());
    parseTreeOutputBuffer += "]";
    while (currentTokenType() == "mult_op" || currentTokenType() == "div_op") {
        std::shared_ptr<TypeStruct> additional;
        // Determine what type of action to carry out
        if (currentTokenType() == "mult_op") {
            parseTreeOutputBuffer += "[TERM ";
            // Do a multiplication
            parseOperator();
            additional = parseFactor();
            // Determine what type of math we need to do
            if (ret->activeType == 'I') {
                if (additional->activeType == 'I') {
                    ret->typeUnion.intType->setValue(
                            ret->typeUnion.intType->getValue() * additional->typeUnion.intType->getValue());
                }
                else {
                    // In this case additional must be a float
                    ret->typeUnion.floatType->setValue(
                            ret->typeUnion.intType->getValue() * additional->typeUnion.floatType->getValue());
                    ret->activeType = 'F';
                }
            }
            else {
                if (additional->activeType != 'I') {
                    ret->typeUnion.floatType->setValue(
                            ret->typeUnion.floatType->getValue() * additional->typeUnion.intType->getValue());
                }
                else {
                    ret->typeUnion.floatType->setValue(
                            ret->typeUnion.floatType->getValue() * additional->typeUnion.floatType->getValue());
                }
            }
            parseTreeOutputBuffer += "]";
        }
        else if (currentTokenType() == "div_op") {
            parseTreeOutputBuffer += "[TERM ";
            // Do a division
            parseOperator();
            additional = parseFactor();
            if (ret->activeType == 'I') {
                if (additional->activeType == 'I') {
                    // If they're both integers then check to see if we can perform an integer division
                    if (ret->typeUnion.intType->getValue() % additional->typeUnion.intType->getValue() == 0) {
                        // Integer division
                        ret->typeUnion.intType->setValue(
                                ret->typeUnion.intType->getValue() / additional->typeUnion.intType->getValue());
                    }
                    else {
                        ret->typeUnion.floatType->setValue(
                                ((float) ret->typeUnion.intType->getValue()) /
                                additional->typeUnion.intType->getValue());
                        ret->activeType = 'F';
                    }
                }
                else {
                    // Integer / float
                    ret->typeUnion.floatType->setValue(
                            ret->typeUnion.intType->getValue() / additional->typeUnion.floatType->getValue());
                    ret->activeType = 'F';
                }
            }
            else {
                if (additional->activeType == 'I') {
                    ret->typeUnion.floatType->setValue(
                            ret->typeUnion.floatType->getValue() / additional->typeUnion.intType->getValue());
                }
                else {
                    ret->typeUnion.floatType->setValue(
                            ret->typeUnion.floatType->getValue() / additional->typeUnion.floatType->getValue());
                }
            }
            parseTreeOutputBuffer += "]";
        }
    }
    return ret;

}

std::shared_ptr<TypeStruct> RuMParser::parseFactor() {
    parseTreeOutputBuffer += "[FACTOR ";
    std::shared_ptr<TypeStruct> ret = std::make_shared<TypeStruct>(parseNeg());
    parseTreeOutputBuffer += "]";
    if (currentTokenType() == "exponent_op") {
        parseOperator();
        std::shared_ptr<TypeStruct> power = parseFactor();
        if (ret->activeType == 'I') {
            if (power->activeType == 'I') {
                ret->typeUnion.intType->setValue((int) std::pow(ret->typeUnion.intType->getValue(),
                                                                power->typeUnion.intType->getValue()));
            }
            else {
                ret->typeUnion.floatType->setValue((float) std::pow(ret->typeUnion.intType->getValue(),
                                                                    power->typeUnion.floatType->getValue()));
                ret->activeType = 'F';
            }
        }
        else {
            if (power->activeType == 'I') {
                ret->typeUnion.floatType->setValue((float) std::pow(ret->typeUnion.floatType->getValue(),
                                                                    power->typeUnion.intType->getValue()));
            }
            else {
                ret->typeUnion.floatType->setValue(std::pow(ret->typeUnion.floatType->getValue(),
                                                            power->typeUnion.floatType->getValue()));
            }
        }
    }
    return ret;

}

std::shared_ptr<TypeStruct> RuMParser::parseNeg() {
    std::shared_ptr<TypeStruct> ret;
    parseTreeOutputBuffer += "[NEG ";
    if (currentTokenType() == "identifier") {
        // In this case it's either a variable or a function invocation
        // If the next token is a '(' it's a function invocation
        if (tokenList->at(tokenListPosition + 1).getTokenType() == "open_paren") {
            ret = parseInvoke();
            if (ret == nullptr) {
                ret = std::make_shared<TypeStruct>();
            }
        }// If the next token is a '.' then it's a class access
        else if (tokenList->at(tokenListPosition + 1).getTokenType() == "dot_op") {
            ret = parseClassAccess();
        }// If it was neither of the previous then it's just a normal variable reference
        else {
            std::string identifier = parseVar();
            ret = this->currentScope->getVariable(identifier);
            if (ret == nullptr) {
                throw std::runtime_error("Reference to undefined variable " + identifier);
            }
            ret = std::make_shared<TypeStruct>(ret);
        }
    }
    else if (currentTokenType() == "float" || currentTokenType() == "integer") {
        ret = parseNum();
    }
    else if (currentTokenType() == "negative_op") {
        // If it starts with a negative sign then we need to multiply the next value by negative 1
        parseOperator();
        // Capture the value and make it negative
        ret = std::make_shared<TypeStruct>(parseNeg());
        if (ret->activeType == 'I') {
            ret->typeUnion.intType->setValue(ret->typeUnion.intType->getValue() * -1);
        }
        else if (ret->activeType == 'F') {
            ret->typeUnion.floatType->setValue(ret->typeUnion.floatType->getValue() * -1);
        }
        else if (ret->activeType == 'B') {
            ret->typeUnion.boolType->setValue(!ret->typeUnion.boolType->getValue());
        }
        else {
            throw std::runtime_error("Tried to use '-' operator on invalid type");
        }
    }
    else if (currentTokenType() == "open_paren") {
        // (<EXPR>)
        parseOperator();
        ret = parseExpr();
        // After parsing the expression we should now be on a ')'
        if (currentTokenType() == "close_paren") {
            parseOperator();
        }
        else {
            throw std::runtime_error("Expected 'close_paren' but instead received '" + currentTokenType() + "'.");
        }
    }
    else {
        throw std::runtime_error(
                "Expected a number, or negative number, or variable, or function call, or '(' but instead received '"
                + currentTokenType() + "'.");
    }
    parseTreeOutputBuffer += "]";
    return ret;
}

void RuMParser::parseList() {
    parseTreeOutputBuffer += "[LIST ";
    if (currentTokenType() == "left_square_bracket") {
        parseOperator();
        parseArgList();
        if (currentTokenType() == "right_square_bracket") {
            parseOperator();
        }
        else {
            throw std::runtime_error(
                    "Expected 'right_square_bracket' but instead received '" + currentTokenType() + "'.");
        }
        parseTreeOutputBuffer += "]";
    }
    else {
        throw std::runtime_error("Expected 'left_square_bracket' but instead received '" + currentTokenType() + "'.");
    }
}

std::string RuMParser::parseVar() {
    parseTreeOutputBuffer += "[VAR ";
    std::string identifier = parseIdentifier();
    Token &token = tokenList->at(tokenListPosition);
    this->lastExpression = identifier;
    parseTreeOutputBuffer += "]";
    return identifier;
}

std::string RuMParser::parseIdentifier() {
    if (currentTokenType() == "identifier") {
        Token &token = tokenList->at(tokenListPosition);
        //parseTreeOutputBuffer += "TEST";
        parseTreeOutputBuffer += "[identifier " + token.getLexeme() + "]";
        this->lastExpression = token.getLexeme();
        ++tokenListPosition;
        return token.getLexeme();
    }
    else {
        throw std::runtime_error("Expected an 'identifier' but instead received '" + currentTokenType() + "'.");
    }
}

std::shared_ptr<TypeStruct> RuMParser::parseString() {
    std::shared_ptr<TypeStruct> ret = std::make_shared<TypeStruct>();
    if (currentTokenType() == "string") {
        Token &token = tokenList->at(tokenListPosition);
        parseTreeOutputBuffer += "[STRING " + token.getLexeme() + "]";
        this->lastExpression = token.getLexeme();
        ++tokenListPosition;
        ret->typeUnion.stringType = new Type<std::string>("string",
                                                          token.getLexeme().substr(1, token.getLexeme().size() - 2));
        ret->activeType = 'S';
    }
    else {
        throw std::runtime_error("Expected a 'string' but instead received '" + currentTokenType() + "'.");
    }
    return ret;
}

std::shared_ptr<TypeStruct> RuMParser::parseNum() {
    std::shared_ptr<TypeStruct> ret;
    if (currentTokenType() == "integer") {
        parseTreeOutputBuffer += "[NUM ";
        ret = parseInt();
        parseTreeOutputBuffer += "]";
    }
    else if (currentTokenType() == "float") {
        parseTreeOutputBuffer += "[NUM ";
        ret = parseFloat();
        parseTreeOutputBuffer += "]";
    }
    else {
        throw std::runtime_error("Expected a 'float' or an 'int' but instead received '" + currentTokenType() + "'.");
    }
    return ret;
}

std::shared_ptr<TypeStruct> RuMParser::parseInt() {
    std::shared_ptr<TypeStruct> ret = std::make_shared<TypeStruct>();
    if (currentTokenType() == "integer") {
        Token &token = tokenList->at(tokenListPosition);
        parseTreeOutputBuffer += "[INT " + token.getLexeme() + "]";
        this->lastExpression = token.getLexeme();
        ++tokenListPosition;
        ret->typeUnion.intType = new Type<int>(token.getTokenType(), std::stoi(token.getLexeme()));
        ret->activeType = 'I';
    }
    else {
        throw std::runtime_error("Expected 'int' and received '" + currentTokenType() + "'.");
    }
    return ret;
}

std::shared_ptr<TypeStruct> RuMParser::parseFloat() {
    std::shared_ptr<TypeStruct> ret = std::make_shared<TypeStruct>();
    if (currentTokenType() == "float") {
        Token &token = tokenList->at(tokenListPosition);
        parseTreeOutputBuffer += "[FLOAT " + token.getLexeme() + "]";
        this->lastExpression = token.getLexeme();
        ++tokenListPosition;
        ret->typeUnion.floatType = new Type<float>(token.getTokenType(), std::stof(token.getLexeme()));
        ret->activeType = 'F';
    }
    else {
        throw std::runtime_error("Expected 'float' and received '" + currentTokenType() + "'.");
    }
    return ret;
}


void RuMParser::parseKeyword() {
    if (keywordParseMap.count(currentTokenType()) != 0) {
        parseTreeOutputBuffer += keywordParseMap[currentTokenType()];
        ++tokenListPosition;
    }
    else {
        throw std::runtime_error(
                "Expected a keyword token but instead received a token of type '" + currentTokenType() + "'.");
    }
}

void RuMParser::parseOperator() {
    if (operatorParseMap.count(currentTokenType()) != 0) {
        parseTreeOutputBuffer += operatorParseMap[currentTokenType()];
        ++tokenListPosition;
    }
    else {
        throw std::runtime_error(
                "Expected an operator token but instead received a token of type '" + currentTokenType() + "'.");
    }
}

void RuMParser::printFunction() {
    // This is to get us over the open paren
    if (currentTokenType() != "open_paren") {
        throw std::runtime_error("RuMParser::printFunction expected '(' but instead received " + currentTokenType());
    }
    parseOperator();
    std::shared_ptr<TypeStruct> value = parseExpr();
    // Get over the close paren
    if (currentTokenType() != "close_paren") {
        throw std::runtime_error("RuMParser::printFunction expected ')' but instead received " + currentTokenType());
    }
    parseOperator();
    this->setLastExpression(value);
    std::cout << lastExpression << std::endl;
}

