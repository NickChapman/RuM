//
// Created by Nick Chapman on 10/27/16.
//

#include "RuMParser.h"
#include "RuM.h"

// We don't actually do anything in this case
// We need a token list in order to do anything
RuMParser::RuMParser() {}

RuMParser::RuMParser(std::shared_ptr<std::vector<Token>> tokenList) {
    this->tokenList = tokenList;
    this->tokenListPosition = 0;
    this->parseTreeOutputBuffer = "";

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
             tokenList->at(tokenListPosition + 1).getTokenType() == "assignment_op") {
        parseAssign();
    }
    else {
        parseArg();
    }
    parseTreeOutputBuffer += "]";
}

void RuMParser::parseAssign() {
    parseTreeOutputBuffer += "[ASSIGN ";
    if (currentTokenType() == "identifier") {
        parseVar();
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

void RuMParser::parseInvoke() {
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


void RuMParser::parseClassAccess() {
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
    while (currentTokenType() == "dop_op") {
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

void RuMParser::parseExpr() {
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

void RuMParser::parseMathExpr() {
    parseTreeOutputBuffer += "[MATH-EXPR ";
    parseTerm();
    parseMathExprPrime();
    parseTreeOutputBuffer += "]";
}

void RuMParser::parseMathExprPrime() {
    // This is used to eliminate left recursion and get appropriate associativity for multiplication and division
    if (currentTokenType() == "plus_op" ||
        currentTokenType() == "negative_op") {
        parseTreeOutputBuffer += "[MATH-EXPR' ";
        parseOperator();
        parseTerm();
        parseTreeOutputBuffer += "]";
    }
}

void RuMParser::parseTerm() {
    parseTreeOutputBuffer += "[TERM ";
    parseFactor();
    parseTermPrime();
    parseTreeOutputBuffer += "]";
}

void RuMParser::parseTermPrime() {
    // This is used to eliminate left recursion and get appropriate associativity for multiplication and division
    if (currentTokenType() == "mult_op" ||
        currentTokenType() == "div_op") {
        parseTreeOutputBuffer += "[TERM' ";
        parseOperator();
        parseFactor();
        parseTreeOutputBuffer += "]";
    }
}

void RuMParser::parseFactor() {
    parseTreeOutputBuffer += "[FACTOR ";
    parseNeg();
    parseFactorPrime();
    parseTreeOutputBuffer += "]";
}

void RuMParser::parseFactorPrime() {
    // This is used to eliminate left recursion and get appropriate associativity for exponentiation
    while (currentTokenType() == "exponent_op") {
        parseTreeOutputBuffer += "[FACTOR' ";
        parseOperator();
        parseNeg();
        parseTreeOutputBuffer += "]";
    }
}

void RuMParser::parseNeg() {
    parseTreeOutputBuffer += "[NEG ";
    if (currentTokenType() == "identifier") {
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
    else if (currentTokenType() == "float" || currentTokenType() == "integer") {
        parseNum();
    }
    else if (currentTokenType() == "negative_op") {
        // -<NEG>
        parseOperator();
        parseNeg();
    }
    else if (currentTokenType() == "open_paren") {
        // (<EXPR>)
        parseOperator();
        parseExpr();
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

void RuMParser::parseVar() {
    parseTreeOutputBuffer += "[VAR ";
    parseIdentifier();
    parseTreeOutputBuffer += "]";
}

void RuMParser::parseIdentifier() {
    if (currentTokenType() == "identifier") {
        parseTreeOutputBuffer += "[identifier " + tokenList->at(tokenListPosition).getLexeme() + "]";
        ++tokenListPosition;
    }
    else {
        throw std::runtime_error("Expected an 'identifier' but instead received '" + currentTokenType() + "'.");
    }
}

void RuMParser::parseString() {
    if (currentTokenType() == "string") {
        parseTreeOutputBuffer += "[STRING " + tokenList->at(tokenListPosition).getLexeme() + "]";
        ++tokenListPosition;
    }
    else {
        throw std::runtime_error("Expected a 'string' but instead received '" + currentTokenType() + "'.");
    }
}

void RuMParser::parseNum() {
    if (currentTokenType() == "integer") {
        parseTreeOutputBuffer += "[NUM ";
        parseInt();
        parseTreeOutputBuffer += "]";
    }
    else if (currentTokenType() == "float") {
        parseTreeOutputBuffer += "[NUM ";
        parseFloat();
        parseTreeOutputBuffer += "]";
    }
    else {
        throw std::runtime_error("Expected a 'float' or an 'int' but instead received '" + currentTokenType() + "'.");
    }
}

void RuMParser::parseInt() {
    if (currentTokenType() == "integer") {
        parseTreeOutputBuffer += "[INT " + tokenList->at(tokenListPosition).getLexeme() + "]";
        ++tokenListPosition;
    }
    else {
        throw std::runtime_error("Expected 'int' and received '" + currentTokenType() + "'.");
    }
}

void RuMParser::parseFloat() {
    if (currentTokenType() == "float") {
        parseTreeOutputBuffer += "[FLOAT " + tokenList->at(tokenListPosition).getLexeme() + "]";
        ++tokenListPosition;
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
