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

void RuMParser::setTokenList(std::vector<Token> *tokenList) {
    this->tokenList = tokenList;
    this->tokenListPosition = 0;
}

// Helper function
std::string RuMParser::currentTokenType() {
    return tokenList->at(tokenListPosition).getTokenType();
}

void RuMParser::reset() {
    this->outputBuffer = "";
    this->tokenListPosition = 0;
}

void RuMParser::parseProgram() {
    outputBuffer += "[PROGRAM ";
    parseStmtList();
    outputBuffer += "]";
    std::cout << "CONSTRUCTED PARSE TREE:" << std::endl;
    std::cout << outputBuffer << std::endl;
    reset();
}

void RuMParser::parseStmtList() {
    outputBuffer += "[STMT-LIST ";
    while (currentTokenType() != "else_key" && currentTokenType() != "endif_key" &&
           currentTokenType() != "endwhile_key" &&
           currentTokenType() != "endfunction_key" &&
           currentTokenType() != "endinput") {
        parseStmt();
    }
    outputBuffer += "]";
}

void RuMParser::parseStmt() {
    //<ARG> | <IF_STMT> | <WHILE> | <FUNC> | <CLASS> | <ASSIGN>
    outputBuffer += "[STMT ";
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
             tokenList->at(tokenListPosition + 1).getTokenType() == "assignment_op") {
        parseAssign();
    }
    else {
        parseArg();
    }
    outputBuffer += "]";
}

void RuMParser::parseAssign() {
    outputBuffer += "[ASSIGN ";
    if (currentTokenType() == "identifier") {
        parseVar();
        if (currentTokenType() == "assignment_op") {
            parseOperator();
            parseArg();
            if (currentTokenType() == "semicolon") {
                parseOperator();
            }
            else {
                throw "Expected ';' but instead received '" + currentTokenType() + "'.";
            }
        }
        else {
            throw "Expected '=' but instead received '" + currentTokenType() + "'.";
        }
    }
    else {
        throw "Expected 'identifier' but instead received '" + currentTokenType() + "'.";
    }
    outputBuffer += "]";
}

void RuMParser::parseFunc() {
    outputBuffer += "[FUNC ";
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
                                    throw "Expected 'identifier' but instead received '" + currentTokenType() + "'.";
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
                                throw "Expected 'endfun' but instead received '" + currentTokenType() + "'.";
                            }
                        }
                        else {
                            throw "Expected ')' but instead received '" + currentTokenType() + "'.";
                        }
                    }
                    else {
                        throw "Expected '(' but instead received '" + currentTokenType() + "'.";
                    }
                }
                else {
                    throw "Expected 'identifier' but instead received '" + currentTokenType() + "'.";
                }
            }
            else {
                throw "Expected '=' but instead received '" + currentTokenType() + "'.";
            }
        }
        else {
            throw "Expected 'variable' but instead received '" + currentTokenType() + "'.";
        }
    }
    else {
        throw "Expected 'fundef' but instead received '" + currentTokenType() + "'.";
    }
    outputBuffer += "]";
}

void RuMParser::parseInvoke() {
    outputBuffer += "[INVOKE ";
    if (currentTokenType() == "identifier") {
        parseIdentifier();
        if (currentTokenType() == "open_paren") {
            parseOperator();
            parseArgList();
            if (currentTokenType() == "close_paren") {
                parseOperator();
            }
            else {
                throw "Expected ')' but instead received '" + currentTokenType() + "'.";
            }
        }
        else {
            throw "Expected '(' but instead received '" + currentTokenType() + "'.";
        }
    }
    else {
        throw "Expected 'identifier' but instead received '" + currentTokenType() + "'.";
    }
    outputBuffer += "]";
}

void RuMParser::parseArgList() {
    outputBuffer += "[ARG-LIST ";
    parseArg();
    while (currentTokenType() == "comma") {
        parseOperator();
        parseArg();
    }
    outputBuffer += "]";
}

void RuMParser::parseArg() {
    outputBuffer += "[ARG ";
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
    outputBuffer += "]";
}

void RuMParser::parseRef() {
    outputBuffer += "[REF ";
    if (currentTokenType() == "reference_op") {
        parseOperator();
        parseVar();
    }
    else {
        throw "Expected '&' but instead received '" + currentTokenType() + "'.";
    }
    outputBuffer += "]";
}

void RuMParser::parseAnonClass() {
    outputBuffer += "[ANON-CLASS";
    if (currentTokenType() == "new_key") {
        parseKeyword();
        if (currentTokenType() == "open_paren") {
            parseOperator();
            if (currentTokenType() == "string") {
                parseString();
                if (currentTokenType() == "close_paren") {
                    parseOperator();
                    outputBuffer += "]";
                }
                else {
                    throw "Expected ')' but instead received '" + currentTokenType() + "'.";
                }
            }
            else {
                throw "Expected 'string' but instead received '" + currentTokenType() + "'.";
            }
        }
        else {
            throw "Expected '(' but instead received '" + currentTokenType() + "'.";
        }
    }
    else {
        throw "Expected 'new' but instead received '" + currentTokenType() + "'.";
    }
}

void RuMParser::parseClass() {
    outputBuffer += "[CLASS ";
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
                    throw "Expected 'from' but instead received '" + currentTokenType() + "'.";
                }
            }
            // Now regardless we have the class block
            parseClassBlock();
            // Now we should be on the endclass keyword
            if (currentTokenType() == "endclass_key") {
                parseKeyword();
            }
            else {
                throw "Expected 'endclass' but instead received '" + currentTokenType() + "'.";
            }
        }
        else {
            throw "Expected 'identifier' but instead received '" + currentTokenType() + "'.";
        }
    }
    else {
        throw "Expected 'classdef' but instead received '" + currentTokenType() + "'.";
    }
    outputBuffer += "]";
}

void RuMParser::parseClassBlock() {
    outputBuffer += "[CLASS-BLOCK ";
    while (currentTokenType() != "endclass_key") {
        // We are not done with the class items
        parseClassItem();
    }
    outputBuffer += "]";
}

void RuMParser::parseClassItem() {
    outputBuffer += "[CLASS-ITEM ";
    if (currentTokenType() == "function_key") {
        // It is a function definition
        parseFunc();
    }
    else {
        // Otherwise it has to be an assignment
        parseAssign();
    }
    outputBuffer += "]";
}


void RuMParser::parseClassAccess() {
    outputBuffer += "[CLASS-ACCESS ";
    if (currentTokenType() == "identifier") {
        parseVar();
        parseClassAccessPrime();
    }
    else {
        throw "Expected 'identifier' but instead recieved '" + currentTokenType() + "'.";
    }
    outputBuffer += "]";
}

void RuMParser::parseClassAccessPrime() {
    while (currentTokenType() == "dop_op") {
        outputBuffer += "[CLASS-ACCESS' ";
        parseOperator();
        if (tokenList->at(tokenListPosition + 1).getTokenType() == "open_paren") {
            // In this case it is a function invocation
            parseInvoke();
        }
        else {
            // Just a normal variable access
            parseVar();
        }
        outputBuffer += "]";
    }
}

void RuMParser::parseWhile() {
    if (currentTokenType() == "while_key") {
        outputBuffer += "[WHILE ";
        parseKeyword();
        if (currentTokenType() == "open_paren") {
            parseOperator();
            parseExpr();
            if (currentTokenType() == "close_paren") {
                parseOperator();
                parseStmtList();
                if (currentTokenType() == "endwhile_key") {
                    parseKeyword();
                    outputBuffer += "]";
                }
                else {
                    throw "Expected 'endwhile' but instead received '" + currentTokenType() + "'.";
                }
            }
            else {
                throw "Expected ')' but instead received '" + currentTokenType() + "'.";
            }
        }
        else {
            throw "Expected '(' but instead received '" + currentTokenType() + "'.";
        }
    }
    else {
        throw "Expected 'while' but instead received '" + currentTokenType() + "'.";
    }
}

void RuMParser::parseIfStmt() {
    if (currentTokenType() == "if_key") {
        outputBuffer += "[IF-STMT ";
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
                    outputBuffer += "]";
                }
                else {
                    throw "Expected `endif_key` but instead received '" + currentTokenType() + "'.";
                }
            }
            else {
                throw "Expected a ')' but instead received '" + currentTokenType() + "'.";
            }
        }
        else {
            throw "Expected a '(' but instead received '" + currentTokenType() + "'.";
        }
    }
    else {
        throw "Expected 'if' but instead received '" + currentTokenType() + "'.";
    }
}

void RuMParser::parseExpr() {
    outputBuffer += "[EXPR ";
    if (currentTokenType() == "bool_not") {
        parseOperator();
        parseExpr();
    }
    else {
        parseBool();
    }
    outputBuffer += "]";
}

void RuMParser::parseBool() {
    outputBuffer += "[BOOL ";
    parseBoolTerm();
    parseBoolPrime();
    outputBuffer += "]";
}

void RuMParser::parseBoolPrime() {
    while (currentTokenType() == "bool_equals" ||
           currentTokenType() == "bool_lessthan" ||
           currentTokenType() == "bool_greaterthan" ||
           currentTokenType() == "bool_and" ||
           currentTokenType() == "bool_or") {
        outputBuffer += "[BOOL' ";
        parseOperator();
        parseBoolTerm();
        outputBuffer += "]";
    }
}

void RuMParser::parseBoolTerm() {
    outputBuffer += "[BOOL-TERM ";
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
    outputBuffer += "]";
}

void RuMParser::parseMathExpr() {
    outputBuffer += "[MATH-EXPR ";
    parseTerm();
    parseMathExprPrime();
    outputBuffer += "]";
}

void RuMParser::parseMathExprPrime() {
    // This is used to eliminate left recursion and get appropriate associativity for multiplication and division
    if (currentTokenType() == "plus_op" ||
        currentTokenType() == "negative_op") {
        outputBuffer += "[MATH-EXPR' ";
        parseOperator();
        parseTerm();
        outputBuffer += "]";
    }
}

void RuMParser::parseTerm() {
    outputBuffer += "[TERM ";
    parseFactor();
    parseTermPrime();
    outputBuffer += "]";
}

void RuMParser::parseTermPrime() {
    // This is used to eliminate left recursion and get appropriate associativity for multiplication and division
    if (currentTokenType() == "mult_op" ||
        currentTokenType() == "div_op") {
        outputBuffer += "[TERM' ";
        parseOperator();
        parseFactor();
        outputBuffer += "]";
    }
}

void RuMParser::parseFactor() {
    outputBuffer += "[FACTOR ";
    parseNeg();
    parseFactorPrime();
    outputBuffer += "]";
}

void RuMParser::parseFactorPrime() {
    // This is used to eliminate left recursion and get appropriate associativity for exponentiation
    while (currentTokenType() == "exponent_op") {
        outputBuffer += "[FACTOR' ";
        parseOperator();
        parseNeg();
        outputBuffer += "]";
    }
}

void RuMParser::parseNeg() {
    Token *currentToken = &(tokenList->at(tokenListPosition));
    outputBuffer += "[NEG ";
    if (currentToken->getTokenType() == "identifier") {
        // In this case it's either a variable or a function invocation
        // If the next token is a '(' it's a function invocation
        if (tokenList->at(tokenListPosition + 1).getTokenType() == "open_paren") {
            parseInvoke();
        }
        else if (tokenList->at(tokenListPosition + 1).getTokenType() == "dot_op") {
            parseClassAccess();
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
        if (currentTokenType() == "close_paren") {
            parseOperator();
        }
        else {
            throw "Expected 'close_paren' but instead received '" + currentToken->getTokenType() + "'.";
        }
    }
    else {
        throw "Expected a number, or negative number, or variable, or function call, or '(' but instead received '"
              + currentToken->getTokenType() + "'.";
    }
    outputBuffer += "]";
}

void RuMParser::parseList() {
    Token *currentToken = &(tokenList->at(tokenListPosition));
    outputBuffer += "[LIST ";
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
        outputBuffer += "]";
    }
    else {
        throw "Expected 'left_square_bracket' but instead received '" + currentToken->getTokenType() + "'.";
    }
}

void RuMParser::parseVar() {
    outputBuffer += "[VAR ";
    parseIdentifier();
    outputBuffer += "]";
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
        outputBuffer += "[STRING " + currentToken->getLexeme() + "]";
        ++tokenListPosition;
    }
    else {
        throw "Expected a 'string' but instead received '" + currentToken->getTokenType() + "'.";
    }
}

void RuMParser::parseNum() {
    Token *currentToken = &(tokenList->at(tokenListPosition));
    if (currentToken->getTokenType() == "integer") {
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
    if (currentToken->getTokenType() == "integer") {
        outputBuffer += "[INT " + currentToken->getLexeme() + "]";
        ++tokenListPosition;
    }
    else {
        throw "Expected 'int' and received '" + currentToken->getTokenType() + "'.";
    }
}

void RuMParser::parseFloat() {
    Token *currentToken = &(tokenList->at(tokenListPosition));
    if (currentToken->getTokenType() == "float") {
        outputBuffer += "[FLOAT " + currentToken->getLexeme() + "]";
        ++tokenListPosition;
    }
    else {
        throw "Expected 'float' and received '" + currentToken->getTokenType() + "'.";
    }
}

void RuMParser::parseKeyword() {
    Token *currentToken = &(tokenList->at(tokenListPosition));
    if (keywordParseMap.count(currentToken->getTokenType()) != 0) {
        outputBuffer += keywordParseMap[currentToken->getTokenType()];
        ++tokenListPosition;
    }
    else {
        throw "Expected a keyword token but instead received a token of type '" + currentToken->getTokenType() + "'.";
    }
}

void RuMParser::parseOperator() {
    Token *currentToken = &(tokenList->at(tokenListPosition));
    if (operatorParseMap.count(currentToken->getTokenType()) != 0) {
        outputBuffer += operatorParseMap[currentToken->getTokenType()];
        ++tokenListPosition;
    }
    else {
        throw "Expected an operator token but instead received a token of type '" + currentToken->getTokenType() + "'.";
    }
}
