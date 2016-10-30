//
// Created by Nick Chapman on 10/27/16.
//

#include "RuMParser.h"
#include "RuM.h"

// We don't actually do anything in this case
// We need a token list in order to do anything
RuMParser::RuMParser() {}

RuMParser::RuMParser(std::vector<Token> *tokenList) {
    this->tokenList = tokenList;
    this->tokenListPosition = 0;
    this->outputBuffer = "";

    // Build our keyword parsing map
    keywordParseMap["if_key"] = "[keyword if]";
    keywordParseMap["endif_key"] = "[keyword endif]";
    keywordParseMap["while_key"] = "[keyword while]";
    keywordParseMap["endwhile_key"] = "[keyword endwhile]";
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
}

void RuMParser::parseFactor() {

}

void RuMParser::parseNeg() {
    Token *currentToken = &(tokenList->at(tokenListPosition));
    if (currentToken->getTokenType() == "identifier") {
        // In this case it's either a variable or a function invocation
        // If the next token is a '(' it's a function invocation
        if (tokenList->at(tokenListPosition + 1).getTokenType() == "open_paren") {
            parseInvoke();
        }
        else {
            parseVar();
        }
    }
    else if (currentToken->getTokenType() == "float" || currentToken->getTokenType() == "integer") {
        parseNum();
    }
    else if (currentToken->getTokenType() == "negative_op") {
        // -<NEG>
        parseOperator();
        parseNeg();
    }
    else if (currentToken->getTokenType() == "open_paren") {
        // (<EXPR>)
        parseOperator();
        parseExpr();
        // After parsing the expression we should now be on a ')'
        if (tokenList->at(tokenListPosition).getTokenType() == "close_paren") {
            parseOperator();
        }
        else {
            throw "Expected 'close_paren' but instead received '" + currentToken->getTokenType() + "'.";
        }
    }
    else {
        throw "Expected a number, or negative number, or variable, or function call, or () but instead received '"
              + currentToken->getTokenType() + "'.";
    }

}

void RuMParser::parseList() {
    Token *currentToken = &(tokenList->at(tokenListPosition));
    if (currentToken->getTokenType() == "left_square_bracket") {
        parseOperator();
        parseArgList();
        currentToken = &(tokenList->at(tokenListPosition));
        if (currentToken->getTokenType() == "right_square_bracket") {
            parseOperator();
        }
        else {
            throw "Expected 'right_square_bracket' but instead received '" + currentToken->getTokenType() + "'.";
        }
    }
    else {
        throw "Expected 'left_square_bracket' but instead received '" + currentToken->getTokenType() + "'.";
    }
}

void RuMParser::parseVar() {
    parseIdentifier();
}

void RuMParser::parseIdentifier() {
    Token *currentToken = &(tokenList->at(tokenListPosition));
    if (currentToken->getTokenType() == "identifier") {
        outputBuffer += "[identifier " + currentToken->getLexeme() + "]";
        ++tokenListPosition;
    }
    else {
        throw "Expected an 'identifier' but instead received '" + currentToken->getTokenType() + "'.";
    }
}

void RuMParser::parseString() {
    Token *currentToken = &(tokenList->at(tokenListPosition));
    if (currentToken->getTokenType() == "string") {
        outputBuffer += "[string " + currentToken->getLexeme() + "]";
        ++tokenListPosition;
    }
    else {
        throw "Expected a 'string' but instead received '" + currentToken->getTokenType() + "'.";
    }
}

void RuMParser::parseNum() {
    Token *currentToken = &(tokenList->at(tokenListPosition));
    if (currentToken->getTokenType() == "int") {
        outputBuffer += "[NUM ";
        parseInt();
        outputBuffer += "]";
    }
    else if (currentToken->getTokenType() == "float") {
        outputBuffer += "[NUM ";
        parseFloat();
        outputBuffer += "]";
    }
    else {
        throw "Expected a 'float' or an 'int' but instead received '" + currentToken->getTokenType() + "'.";
    }
}

void RuMParser::parseInt() {
    Token *currentToken = &(tokenList->at(tokenListPosition));
    if (currentToken->getTokenType() == "int") {
        outputBuffer += "[int " + currentToken->getLexeme() + "]";
        ++tokenListPosition;
    }
    else {
        throw "Expected 'int' and received '" + currentToken->getTokenType() + "'.";
    }
}

void RuMParser::parseFloat() {
    Token *currentToken = &(tokenList->at(tokenListPosition));
    if (currentToken->getTokenType() == "float") {
        outputBuffer += "[float " + currentToken->getLexeme() + "]";
        ++tokenListPosition;
    }
    else {
        throw "Expected 'float' and received '" + currentToken->getTokenType() + "'.";
    }

}

void RuMParser::parseKeyword() {
    Token *currentToken = &(tokenList->at(tokenListPosition));
    if (operatorParseMap.find(currentToken->getTokenType()) != operatorParseMap.end()) {
        outputBuffer += keywordParseMap[currentToken->getTokenType()];
        ++tokenListPosition;
    }
    else {
        throw "Expected a keyword token but instead received a token of type '" + currentToken->getTokenType() + "'.";
    }
}

void RuMParser::parseOperator() {
    Token *currentToken = &(tokenList->at(tokenListPosition));
    if (operatorParseMap.find(currentToken->getTokenType()) != operatorParseMap.end()) {
        outputBuffer += operatorParseMap[currentToken->getTokenType()];
        ++tokenListPosition;
    }
    else {
        throw "Expected an operator token but instead received a token of type '" + currentToken->getTokenType() + "'.";
    }
}
