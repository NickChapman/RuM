//
// Created by Nick Chapman on 10/27/16.
//

#ifndef RUM_RUMPARSER_H
#define RUM_RUMPARSER_H


#include <vector>
#include <string>
#include <unordered_map>
#include "Token.h"
#include "Type.h"
#include "Scope.h"

class RuMParser {
    std::shared_ptr<std::vector<Token>> tokenList;
    unsigned int tokenListPosition;
    std::string parseTreeOutputBuffer;
    bool displayParseTree = false;
    std::unordered_map<std::string, std::string> keywordParseMap;
    std::unordered_map<std::string, std::string> operatorParseMap;
    std::string lastExpression;

    Scope globalScope;

    // Helper functions
    std::string currentTokenType();

    void setLastExpression(std::shared_ptr<type_union> expression);

    void parseStmtList();

    void parseStmt();

    std::string parseAssign();

    void parseFunc();

    std::shared_ptr<type_union> parseInvoke();

    void parseArgList();

    void parseArg();

    void parseRef();

    void parseAnonClass();

    void parseClass();

    void parseClassBlock();

    void parseClassItem();

    std::shared_ptr<type_union> parseClassAccess();

    void parseClassAccessPrime();

    void parseWhile();

    void parseIfStmt();

    std::shared_ptr<type_union> parseExpr();

    void parseBool();

    void parseBoolPrime();

    void parseBoolTerm();

//    void parseMathExpr();
//
//    void parseMathExprPrime();
//
//    void parseTerm();
//
//    void parseTermPrime();
//
//    void parseFactor();
//
//    void parseFactorPrime();

    std::shared_ptr<type_union> parseMathExpr();

    std::shared_ptr<type_union> parseMathExprPrime();

    std::shared_ptr<type_union> parseTerm();

    //std::shared_ptr<type_union> parseTermPrime();

    std::shared_ptr<type_union> parseFactor();

    std::shared_ptr<type_union> parseFactorPrime();

    std::shared_ptr<type_union> parseNeg();

    void parseList();

    std::string parseVar();

    std::string parseIdentifier();

    std::shared_ptr<Type<std::string>> parseString();

    std::shared_ptr<type_union> parseNum();

    std::shared_ptr<Type<int>> parseInt();

    std::shared_ptr<Type<float>> parseFloat();

    void parseKeyword();

    void parseOperator();

public:

    RuMParser();

    RuMParser(std::shared_ptr<std::vector<Token>> tokenList);

    void setTokenList(std::shared_ptr<std::vector<Token>> tokenList);

    void setDisplayParseTree(bool display);

    bool getDisplayParseTree() const;

    void parseProgram();

    void reset();
};


#endif //RUM_RUMPARSER_H
