//
// Created by Nick Chapman on 10/15/16.
//

#ifndef PROJECT2_RUM_H
#define PROJECT2_RUM_H

#include <vector>
#include <iostream>
#include <map>
#include "Token.h"

class RuMInterpreter {
    // Interpreter Constants
    const int MAX_INPUT_SIZE = 50000;
    const int MAX_OUTPUT_SIZE = 50000;
    const char *INTERPRETER_PROMPT_NEW = "RuM>";
    const char *INTERPRETER_PROMPT_CONTINUED = ">>>";
    bool isInteractiveMode = false;

    char *inputBuffer = new char[MAX_INPUT_SIZE];
    char *currentCharacter = inputBuffer;
    char *outputBuffer = new char[MAX_OUTPUT_SIZE];
    char *outputPosition = outputBuffer;
    std::vector<Token> tokenList;
    int tokenListPosition = 0;

    void tokenize();

    void fillInputBuffer();

    void getNextNonWhitespace();

    bool isWhiteSpace(char* character);

    bool isIdentifierCharacter(char* character);

    bool isDigit(char* character);

    void ignoreComment();


    bool numericLiteral();

    void stringLiteral();

    bool specialToken();

    bool operatorToken(std::string tokenCharacters);

    bool keywordToken(std::string tokenCharacters);

    bool identifier();

    std::map<std::string, std::string> keywordMap;
    std::map<std::string, std::string> operatorMap;


public:
    RuMInterpreter();

    // Interpreter Types
    void interactiveMode();

    void fileMode(char *filename);


};

#endif //PROJECT2_RUM_H
