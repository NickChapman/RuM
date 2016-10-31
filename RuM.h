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
    bool isInteractiveMode;

    char *inputBuffer;
    char *currentCharacter;
    char *outputBuffer;
    char *outputPosition;
    std::vector<Token> tokenList;
    int tokenListPosition;
    RuMParser parser;

    void tokenize();

    void parse();

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

    static bool isWhiteSpace(char *character);

    static bool isIdentifierCharacter(char *character);

    static bool isDigit(char *character);

    // Interpreter Types
    void interactiveMode();

    void fileMode(char *filename);
};

#endif //PROJECT2_RUM_H
