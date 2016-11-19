//
// Created by Nick Chapman on 10/27/16.
//

#ifndef RUM_RUMPARSER_H
#define RUM_RUMPARSER_H


#include <vector>
#include <string>
#include <unordered_map>
#include "Token.h"

class RuMParser {
    std::shared_ptr<std::vector<Token>> tokenList;
    unsigned int tokenListPosition;
    std::string outputBuffer;
    std::unordered_map<std::string, std::string> keywordParseMap;
    std::unordered_map<std::string, std::string> operatorParseMap;

    // Helper functions
    std::string currentTokenType();

    void parseStmtList();
    void parseStmt();
    void parseAssign();

    void parseFunc();
    void parseInvoke();
    void parseArgList();
    void parseArg();
    void parseRef();
    void parseAnonClass();
    void parseClass();

    void parseClassBlock();
    void parseClassItem();
    void parseClassAccess();
    void parseClassAccessPrime();
    void parseWhile();

    void parseIfStmt();
    void parseExpr();
    void parseBool();
    void parseBoolPrime();
    void parseBoolTerm();
    void parseMathExpr();

    void parseMathExprPrime();
    void parseTerm();
    void parseTermPrime();
    void parseFactor();
    void parseFactorPrime();
    void parseNeg();
    void parseList();

    void parseVar();
    void parseIdentifier();
    void parseString();
    void parseNum();
    void parseInt();
    void parseFloat();
    void parseKeyword();
    void parseOperator();
public:

    RuMParser();
    RuMParser(std::shared_ptr<std::vector<Token>> tokenList);
    void setTokenList(std::shared_ptr<std::vector<Token>> tokenList);
    void parseProgram();
    void reset();
};


#endif //RUM_RUMPARSER_H
