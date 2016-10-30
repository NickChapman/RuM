//
// Created by Nick Chapman on 10/27/16.
//

#ifndef RUM_RUMPARSER_H
#define RUM_RUMPARSER_H


#include <vector>
#include <string>
#include <map>
#include "Token.h"

class RuMParser {
    std::vector<Token>* tokenList;
    unsigned int tokenListPosition;
    std::string outputBuffer;
    std::map<std::string, std::string> keywordParseMap;
    std::map<std::string, std::string> operatorParseMap;

    void parseArgList();
    void parseInvoke();
    void parseExpr();

    void parseFactor();
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
