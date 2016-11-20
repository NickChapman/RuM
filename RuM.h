//
// Created by Nick Chapman on 10/15/16.
//

#ifndef PROJECT2_RUM_H
#define PROJECT2_RUM_H

#include <vector>
#include <iostream>
#include <unordered_map>
#include "Token.h"
#include "RuMParser.h"

class RuMInterpreter {
    // Interpreter Constants
    static const int MAX_INPUT_SIZE = 50000;
    static const int MAX_OUTPUT_SIZE = 50000;
    static const char END_CHAR = '$';
    const char *INTERPRETER_PROMPT_NEW;
    const char *INTERPRETER_PROMPT_CONTINUED;
    bool displayTokenization = false;
    bool isInteractiveMode;

    std::string inputBuffer;
    unsigned long currentCharacterIndex;
    char *outputBuffer;
    char *outputPosition;
    std::shared_ptr<std::vector<Token>> tokenList;
    int tokenListPosition;
    RuMParser parser;

    void tokenize();

    void parse();

    const char &currentCharacter() const;

    void fillInputBuffer();

    void getNextNonWhitespace();

    void ignoreComment();

    bool numericLiteral();

    void stringLiteral();

    bool specialToken();

    bool operatorToken(std::string tokenCharacters);

    bool keywordToken(std::string tokenCharacters);

    bool identifier();

    std::unordered_map<std::string, std::string> keywordMap;

    std::unordered_map<std::string, std::string> operatorMap;
public:


    RuMInterpreter();

    static bool isWhiteSpace(const char &character);

    static bool isIdentifierCharacter(const char &character);

    static bool isDigit(const char &character);

    // Interpreter Types
    void interactiveMode();

    void fileMode(char *filename);

    // Interpreter behaviors
    void setDisplayParseTree(bool display);

    void setDisplayTokenization(bool display);
};

#endif //PROJECT2_RUM_H
