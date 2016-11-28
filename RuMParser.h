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

    void setLastExpression(std::shared_ptr<TypeStruct>& expression);

    void parseStmtList();

    void parseStmt();

    std::string parseAssign();

    void parseFunc();

    std::shared_ptr<TypeStruct> parseInvoke();

    void parseArgList();

    void parseArg();

    void parseRef();

    void parseAnonClass();

    void parseClass();

    void parseClassBlock();

    void parseClassItem();

    std::shared_ptr<TypeStruct> parseClassAccess();

    void parseClassAccessPrime();

    void parseWhile();

    void parseIfStmt();

    std::shared_ptr<TypeStruct> parseExpr();

    void parseBool();

    void parseBoolPrime();

    std::shared_ptr<TypeStruct> parseBoolTerm();

    std::shared_ptr<TypeStruct> parseMathExpr();

    std::shared_ptr<TypeStruct> parseTerm();

    std::shared_ptr<TypeStruct> parseFactor();

    std::shared_ptr<TypeStruct> parseNeg();

    void parseList();

    std::string parseVar();

    std::string parseIdentifier();

    std::shared_ptr<TypeStruct> parseString();

    std::shared_ptr<TypeStruct> parseNum();

    std::shared_ptr<TypeStruct> parseInt();

    std::shared_ptr<TypeStruct> parseFloat();

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
