//
// Created by Nick Chapman on 10/27/16.
//

#include "RuMParser.h"
#include "RuM.h"
#include "Type.h"

// We don't actually do anything in this case
// We need a token list in order to do anything
RuMParser::RuMParser() {}

RuMParser::RuMParser(std::shared_ptr<std::vector<Token>> tokenList) {
    this->tokenList = tokenList;
    this->tokenListPosition = 0;
    this->parseTreeOutputBuffer = "";
    this->lastExpression = "";

    this->globalScope = Scope();

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

void RuMParser::setLastExpression(std::shared_ptr<type_union> expression) {
    if (expression->intType != nullptr) {
        lastExpression = std::to_string(expression->intType->getValue());
    }
    else if (expression->floatType != nullptr) {
        lastExpression = std::to_string(expression->floatType->getValue());
    }
    else if (expression->boolType != nullptr) {
        lastExpression = std::to_string(expression->boolType->getValue());
    }
    else if (expression->stringType != nullptr) {
        lastExpression = expression->stringType->getValue();
    }
    else {
        lastExpression = "nullptr";
    }
}

void RuMParser::reset() {
    this->parseTreeOutputBuffer = "";
    this->tokenListPosition = 0;
}

void RuMParser::parseProgram() {
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
    parseTreeOutputBuffer += "[ASSIGN ";
    if (currentTokenType() == "identifier") {
        std::string identifier = parseVar();
        if (currentTokenType() == "assignment_op") {
            parseOperator();
            parseArg();
            if (currentTokenType() == "semicolon") {
                parseOperator();
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
}

void RuMParser::parseFunc() {
    parseTreeOutputBuffer += "[FUNC ";
    if (currentTokenType() == "function_key") {
        parseKeyword();
        if (currentTokenType() == "identifier") {
            parseVar();
            if (currentTokenType() == "assignment_op") {
                parseOperator();
                if (currentTokenType() == "identifier") {
                    parseIdentifier();
                    if (currentTokenType() == "open_paren") {
                        parseOperator();
                        // Parse a list of vars
                        while (currentTokenType() == "identifier") {
                            parseVar();
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
                            parseStmtList();
                            if (currentTokenType() == "endfunction_key") {
                                parseKeyword();
                            }
                            else {
                                throw std::runtime_error(
                                        "Expected 'endfun' but instead received '" + currentTokenType() + "'.");
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

std::shared_ptr<type_union> RuMParser::parseInvoke() {
    parseTreeOutputBuffer += "[INVOKE ";
    if (currentTokenType() == "identifier") {
        parseIdentifier();
        if (currentTokenType() == "open_paren") {
            parseOperator();
            parseArgList();
            if (currentTokenType() == "close_paren") {
                parseOperator();
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
        throw std::runtime_error("Expected 'identifier' but instead received '" + currentTokenType() + "'.");
    }
    parseTreeOutputBuffer += "]";
}

void RuMParser::parseArgList() {
    parseTreeOutputBuffer += "[ARG-LIST ";
    parseArg();
    while (currentTokenType() == "comma") {
        parseOperator();
        parseArg();
    }
    parseTreeOutputBuffer += "]";
}

void RuMParser::parseArg() {
    parseTreeOutputBuffer += "[ARG ";
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
        parseExpr();
    }
    parseTreeOutputBuffer += "]";
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

std::shared_ptr<type_union> RuMParser::parseClassAccess() {
    parseTreeOutputBuffer += "[CLASS-ACCESS ";
    if (currentTokenType() == "identifier") {
        parseVar();
        parseClassAccessPrime();
    }
    else {
        throw std::runtime_error("Expected 'identifier' but instead recieved '" + currentTokenType() + "'.");
    }
    parseTreeOutputBuffer += "]";
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
    if (currentTokenType() == "while_key") {
        parseTreeOutputBuffer += "[WHILE ";
        parseKeyword();
        if (currentTokenType() == "open_paren") {
            parseOperator();
            parseExpr();
            if (currentTokenType() == "close_paren") {
                parseOperator();
                parseStmtList();
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
    if (currentTokenType() == "if_key") {
        parseTreeOutputBuffer += "[IF-STMT ";
        parseKeyword();
        if (currentTokenType() == "open_paren") {
            parseOperator();
            parseExpr();
            // Check to make sure there is a closing paren
            if (currentTokenType() == "close_paren") {
                parseOperator();
                parseStmtList();
                // Check to see if we finished on an else key
                if (currentTokenType() == "else_key") {
                    parseKeyword();
                    parseStmtList();
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

std::shared_ptr<type_union> RuMParser::parseExpr() {
    parseTreeOutputBuffer += "[EXPR ";
    if (currentTokenType() == "bool_not") {
        parseOperator();
        parseExpr();
    }
    else {
        parseBool();
    }
    parseTreeOutputBuffer += "]";
}

void RuMParser::parseBool() {
    parseTreeOutputBuffer += "[BOOL ";
    parseBoolTerm();
    parseBoolPrime();
    parseTreeOutputBuffer += "]";
}

void RuMParser::parseBoolPrime() {
    while (currentTokenType() == "bool_equals" ||
           currentTokenType() == "bool_lessthan" ||
           currentTokenType() == "bool_greaterthan" ||
           currentTokenType() == "bool_and" ||
           currentTokenType() == "bool_or") {
        parseTreeOutputBuffer += "[BOOL' ";
        parseOperator();
        parseBoolTerm();
        parseTreeOutputBuffer += "]";
    }
}

void RuMParser::parseBoolTerm() {
    parseTreeOutputBuffer += "[BOOL-TERM ";
    if (currentTokenType() == "true_key" ||
        currentTokenType() == "false_key" ||
        currentTokenType() == "null_key") {
        parseKeyword();
    }
    else if (currentTokenType() == "string") {
        parseString();
    }
    else {
        parseMathExpr();
    }
    parseTreeOutputBuffer += "]";
}

std::shared_ptr<type_union> RuMParser::parseMathExpr() {
    parseTreeOutputBuffer += "[MATH-EXPR ";
    parseTerm();
    parseMathExprPrime();
    parseTreeOutputBuffer += "]";
}

std::shared_ptr<type_union> RuMParser::parseMathExprPrime() {
    // This is used to eliminate left recursion and get appropriate associativity for multiplication and division
    if (currentTokenType() == "plus_op" ||
        currentTokenType() == "negative_op") {
        parseTreeOutputBuffer += "[MATH-EXPR' ";
        parseOperator();
        parseTerm();
        parseTreeOutputBuffer += "]";
    }
}

std::shared_ptr<type_union> RuMParser::parseTerm() {
    try {
        parseTreeOutputBuffer += "[TERM ";
        std::shared_ptr<type_union> base = parseFactor();
        parseTreeOutputBuffer += "]";
        while (currentTokenType() == "mult_op" || currentTokenType() == "div_op") {
            std::shared_ptr<type_union> additional;
            // Determine what type of action to carry out
            if (currentTokenType() == "mult_op") {
                parseTreeOutputBuffer += "[TERM ";
                // Do a multiplication
                parseOperator();
                additional = parseFactor();
                // Determine what type of math we need to do
                if (base->intType != nullptr) {
                    if (additional->intType != nullptr) {
                        base->intType->setValue(base->intType->getValue() * additional->intType->getValue());
                    }
                    else {
                        // if (additional->floatType != nullptr)
                        base->floatType->setValue(base->intType->getValue() * additional->floatType->getValue());
                        base->intType = nullptr;
                    }
                }
                else {
                    // if (base->floatType != nullptr)
                    if (additional->intType != nullptr) {
                        base->floatType->setValue(base->floatType->getValue() * additional->intType->getValue());
                    }
                    else {
                        //if (additional->floatType != nullptr)
                        base->floatType->setValue(base->floatType->getValue() * additional->floatType->getValue());
                    }
                }
                parseTreeOutputBuffer += "]";
            }
            else if (currentTokenType() == "div_op") {
                parseTreeOutputBuffer += "[TERM ";
                // Do a division
                parseOperator();
                additional = parseFactor();
                if (base->intType != nullptr) {
                    if (additional->intType != nullptr) {
                        // If they're both integers then check to see if we can perform an integer division
                        if (base->intType->getValue() % additional->intType->getValue() == 0) {
                            // Integer division
                            base->intType->setValue(base->intType->getValue() / additional->intType->getValue());
                        }
                        else {
                            base->floatType->setValue(
                                    ((float) base->intType->getValue()) / additional->intType->getValue());
                            base->intType = nullptr;
                        }
                    }
                    else {
                        // if (additional->floatType != nullptr)
                        // Integer / float
                        base->floatType->setValue(base->intType->getValue() / additional->floatType->getValue());
                        base->intType = nullptr;
                    }
                }
                else {
                    //if (base->floatType != nullptr)
                    if (additional->intType != nullptr) {
                        base->floatType->setValue(base->floatType->getValue() / additional->intType->getValue());
                    }
                    else {
                        //if (additional->floatType != nullptr)
                        base->floatType->setValue(base->floatType->getValue() / additional->floatType->getValue());
                    }
                }
            }
            // Otherwise stop
            parseTreeOutputBuffer += "]";
        }
        setLastExpression(base);
        return base;
    }
        // If we reach this catch statement then that means that someone tried to do a multiplication or division with a string
    catch (std::runtime_error e) {
        throw std::runtime_error(
                "RuMParser::parseTerm: Expected integer or float math but instead received string or boolean.");
    }
}

//std::shared_ptr<type_union> RuMParser::parseTermPrime() {
//    // This is used to eliminate left recursion and get appropriate associativity for multiplication and division
//    while (currentTokenType() == "mult_op" || currentTokenType() == "div_op") {
//        parseTreeOutputBuffer += "[TERM' ";
//        parseOperator();
//        parseFactor();
//        parseTreeOutputBuffer += "]";
//    }
//}

std::shared_ptr<type_union> RuMParser::parseFactor() {
    parseTreeOutputBuffer += "[FACTOR ";
    std::shared_ptr<type_union> factorRet = parseNeg();
    // Now we need to determine what we need to do next
    std::string nextOperation = currentTokenType();
    std::shared_ptr<type_union> factorPrimeRet = parseFactorPrime();
    if (factorPrimeRet != nullptr) {
        if (factorRet->intType != nullptr) {
            if (factorPrimeRet->intType != nullptr) {
                factorRet->intType->setValue(
                        (int) std::pow(factorRet->intType->getValue(), factorPrimeRet->intType->getValue()));
            }
            else if (factorPrimeRet->floatType != nullptr) {
                factorRet->floatType->setValue(
                        (float) std::pow(factorRet->intType->getValue(), factorPrimeRet->floatType->getValue()));
                factorRet->intType = nullptr;
            }
            else {
                throw std::runtime_error(
                        "RuMParser::parseFactor: Expected integer or float math but found string or boolean");
            }
        }
        else if (factorRet->floatType != nullptr) {
            if (factorPrimeRet->intType) {
                factorRet->floatType->setValue(
                        (float) std::pow(factorRet->floatType->getValue(), factorPrimeRet->intType->getValue()));
            }
            else if (factorPrimeRet->floatType != nullptr) {
                factorRet->floatType->setValue(
                        std::pow(factorRet->floatType->getValue(), factorPrimeRet->floatType->getValue()));
            }
            else {
                throw std::runtime_error(
                        "RuMParser::parseFactor: Expected integer or float math but found string or boolean");
            }
        }
        else {
            throw std::runtime_error(
                    "RuMParser::parseFactor: Expected integer or float math but found string or boolean");
        }
    }
    parseTreeOutputBuffer += "]";
    setLastExpression(factorRet);
    return factorRet;
}

std::shared_ptr<type_union> RuMParser::parseFactorPrime() {
    // TODO: THIS IS NOT GOING TO WORK
    // This is used to eliminate left recursion and get appropriate associativity for exponentiation
    std::shared_ptr<type_union> base = nullptr;
    if (currentTokenType() == "exponent_op") {
        parseTreeOutputBuffer += "[FACTOR' ";
        parseOperator();
        base = parseNeg();
        parseTreeOutputBuffer += "]";
    }
    while (currentTokenType() == "exponent_op") {
        std::shared_ptr<type_union> additional;
        parseTreeOutputBuffer += "[FACTOR' ";
        parseOperator();
        additional = parseNeg();
        // Incorporate this into the base
        if (base->intType != nullptr) {
            if (additional->intType != nullptr) {
                base->intType->setValue((int) std::pow(base->intType->getValue(), additional->intType->getValue()));
            }
            else if (additional->floatType != nullptr) {
                base->floatType->setValue((float) std::pow(base->intType->getValue(), additional->floatType->getValue()));
                base->intType = nullptr;
            }
            else {
                throw std::runtime_error(
                        "RuMParser::parseFactorPrime: Expected integer or float math but instead received string or boolean.");
            }
        }
        else if (base->floatType != nullptr) {
            if (additional->intType != nullptr) {
                base->floatType->setValue((float) std::pow(base->intType->getValue(), additional->intType->getValue()));
            }
            else if (additional->floatType != nullptr) {
                base->floatType->setValue((float) std::pow(base->intType->getValue(), additional->floatType->getValue()));
            }
            else {
                throw std::runtime_error(
                        "RuMParser::parseFactorPrime: Expected integer or float math but instead received string or boolean.");
            }
        }
        parseTreeOutputBuffer += "]";
    }
    return base;
}

std::shared_ptr<type_union> RuMParser::parseNeg() {
    // TODO: NEG NEEDS A MAJOR OVERHAUL
    std::shared_ptr<type_union> ret = std::make_shared<type_union>();
    parseTreeOutputBuffer += "[NEG ";
    if (currentTokenType() == "identifier") {
        // In this case it's either a variable or a function invocation
        // If the next token is a '(' it's a function invocation
        if (tokenList->at(tokenListPosition + 1).getTokenType() == "open_paren") {
            ret = parseInvoke();
        }// If the next token is a '.' then it's a class access
        else if (tokenList->at(tokenListPosition + 1).getTokenType() == "dot_op") {
            ret = parseClassAccess();
        }// If it was neither of the previous then it's just a normal variable reference
        else {
            std::string identifier = parseVar();
            ret = this->globalScope.getVariable(identifier);
            if (ret == nullptr) {
                throw std::runtime_error("Reference to undefined variable " + identifier);
            }
        }
    }
        // Now we check to see if it's just a standard numeric type
    else if (currentTokenType() == "float" || currentTokenType() == "integer") {
        ret = parseNum();
    }
        // If it starts with a negative sign then we need to multiply the next value by negative 1
    else if (currentTokenType() == "negative_op") {
        // -<NEG>
        parseOperator();
        // Capture the value and make it negative
        ret = parseNeg();
        if (ret->intType != nullptr) {
            ret->intType->setValue(ret->intType->getValue() * -1);
        }
        else if (ret->floatType != nullptr) {
            ret->floatType->setValue(ret->floatType->getValue() * -1);
        }
        else {
            throw std::runtime_error(
                    "Inside parseNeg, expected a Type<int> or Type<float> but received something else");
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
    setLastExpression(ret); // TODO: This can be taken out and only set at the program level
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
        parseTreeOutputBuffer += "[identifier " + token.getLexeme() + "]";
        this->lastExpression = token.getLexeme();
        ++tokenListPosition;
        return token.getLexeme();
    }
    else {
        throw std::runtime_error("Expected an 'identifier' but instead received '" + currentTokenType() + "'.");
    }
}

std::shared_ptr<Type<std::string>> RuMParser::parseString() {
    if (currentTokenType() == "string") {
        Token &token = tokenList->at(tokenListPosition);
        parseTreeOutputBuffer += "[STRING " + token.getLexeme() + "]";
        this->lastExpression = token.getLexeme();
        ++tokenListPosition;
        return std::shared_ptr<Type<std::string>>(new Type<std::string>(token.getTokenType(), token.getLexeme()));
    }
    else {
        throw std::runtime_error("Expected a 'string' but instead received '" + currentTokenType() + "'.");
    }
}

std::shared_ptr<type_union> RuMParser::parseNum() {
    std::shared_ptr<type_union> ret = std::make_shared<type_union>(new type_union());
    if (currentTokenType() == "integer") {
        parseTreeOutputBuffer += "[NUM ";
        ret->intType = parseInt();
        parseTreeOutputBuffer += "]";
    }
    else if (currentTokenType() == "float") {
        parseTreeOutputBuffer += "[NUM ";
        ret->floatType = parseFloat();
        parseTreeOutputBuffer += "]";
    }
    else {
        throw std::runtime_error("Expected a 'float' or an 'int' but instead received '" + currentTokenType() + "'.");
    }
    return ret;
}

std::shared_ptr<Type<int>> RuMParser::parseInt() {
    if (currentTokenType() == "integer") {
        Token &token = tokenList->at(tokenListPosition);
        parseTreeOutputBuffer += "[INT " + token.getLexeme() + "]";
        this->lastExpression = token.getLexeme();
        ++tokenListPosition;
        return std::make_shared<Type<int>>(token.getTokenType(), std::stoi(token.getLexeme()));
    }
    else {
        throw std::runtime_error("Expected 'int' and received '" + currentTokenType() + "'.");
    }
}

std::shared_ptr<Type<float>> RuMParser::parseFloat() {
    if (currentTokenType() == "float") {
        Token &token = tokenList->at(tokenListPosition);
        parseTreeOutputBuffer += "[FLOAT " + token.getLexeme() + "]";
        this->lastExpression = token.getLexeme();
        ++tokenListPosition;
        return std::shared_ptr<Type<float>>(new Type<float>(token.getTokenType(), std::stof(token.getLexeme())));
    }
    else {
        throw std::runtime_error("Expected 'float' and received '" + currentTokenType() + "'.");
    }
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
