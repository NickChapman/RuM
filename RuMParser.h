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
    std::vector<Token>* tokenList;
    unsigned int tokenListPosition;
    std::string outputBuffer;
    std::unordered_map<std::string, std::string> keywordParseMap;
    std::unordered_map<std::string, std::string> operatorParseMap;

    // TODO: Put these into their correct spots
    void parseArgList();
    void parseInvoke();
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
    RuMParser(std::vector<Token>* tokenList);
};


#endif //RUM_RUMPARSER_H
