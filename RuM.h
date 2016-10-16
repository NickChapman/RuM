//
// Created by Nick Chapman on 10/15/16.
//

#ifndef PROJECT2_RUM_H
#define PROJECT2_RUM_H

#include <vector>
#include <iostream>
#include "Token.h"

class RuMInterpreter {
    // Interpreter Constants
    const int MAX_INPUT_SIZE = 50000;
    const int MAX_OUTPUT_SIZE = 50000;
    const char* INTERPRETER_PROMPT_NEW = "RuM>";
    const char* INTERPRETER_PROMPT_CONTINUED = ">>>";

    char* inputBuffer = new char[MAX_INPUT_SIZE];
    char* currentCharacter;
    char* outputBuffer = new char[MAX_OUTPUT_SIZE];
    char* outputPosition = outputBuffer;
    std::vector<Token> tokenList;

    void tokenize();
    void fillInputBuffer();

    void getNextNonWhitespace();
    bool isWhiteSpace(char* character);
    void ignoreComment();


    void stringLiteral();
    void numericLiteral();
    void operatorCharacter();
    void identifier();


public:
    RuMInterpreter();

    // Interpreter Types
    void interactiveMode();
    void fileMode(char* filename);


};

#endif //PROJECT2_RUM_H
