//
// Created by Nick Chapman on 10/15/16.
//

#include "RuM.h"
#include <fstream>

#define NULL_CHAR '\0'

RuMInterpreter::RuMInterpreter() {
    // Setting constant values
    this->INTERPRETER_PROMPT_NEW = "RuM>";
    this->INTERPRETER_PROMPT_CONTINUED = ">>>";

    // Initializing other interpreter values
    this->isInteractiveMode = false;
    inputBuffer = new char[MAX_INPUT_SIZE];
    currentCharacter = inputBuffer;
    outputBuffer = new char[MAX_OUTPUT_SIZE];
    outputPosition = outputBuffer;
    tokenListPosition = 0;

    // We have to null out the buffers so they don't contain junk values
    for (int i = 0; i < MAX_INPUT_SIZE; ++i) {
        inputBuffer[i] = NULL_CHAR;
    }
    for (int i = 0; i < MAX_OUTPUT_SIZE; ++i) {
        outputBuffer[i] = NULL_CHAR;
    }
    // Fill in our keyword map
    keywordMap["if"] = "if_key";
    keywordMap["else"] = "else_key";
    keywordMap["endif"] = "endif_key";
    keywordMap["while"] = "while_key";
    keywordMap["endwhile"] = "endwhile_key";
    keywordMap["new"] = "new_key";
    keywordMap["classdef"] = "classdef_key";
    keywordMap["inherits"] = "inherits_key";
    keywordMap["from"] = "from_key";
    keywordMap["endclass"] = "endclass_key";
    keywordMap["fundef"] = "function_key";
    keywordMap["endfun"] = "endfunction_key";
    keywordMap["TRUE"] = "true_key";
    keywordMap["FALSE"] = "false_key";
    keywordMap["NULL"] = "null_key";
    keywordMap["exit"] = "exit_token";

    // Fill in our operator map
    operatorMap["-"] = "negative_op";
    operatorMap["+"] = "plus_op";
    operatorMap["*"] = "mult_op";
    operatorMap["/"] = "div_op";
    operatorMap["^"] = "exponent_op";
    operatorMap["="] = "assignment_op";
    operatorMap["=="] = "bool_equals";
    operatorMap["<"] = "bool_lessthan";
    operatorMap[">"] = "bool_greaterthan";
    operatorMap["&&"] = "bool_and";
    operatorMap["||"] = "bool_or";
    operatorMap["!"] = "bool_not";
    operatorMap["&"] = "reference_op";
    operatorMap["("] = "open_paren";
    operatorMap[")"] = "close_paren";
    operatorMap["["] = "left_square_bracket";
    operatorMap["]"] = "right_square_bracket";
    operatorMap[","] = "comma";

    // Finally we tie in our parser
    this->parser = RuMParser(std::shared_ptr<std::vector<Token>>(&tokenList));
}

void RuMInterpreter::getNextNonWhitespace() {
    while (isWhiteSpace(currentCharacter)) {
        ++currentCharacter;
    }
}

bool RuMInterpreter::isWhiteSpace(char *character) {
    return (*character == ' ' || *character == '\n' || *character == '\r' || *character == '\t');
}

bool RuMInterpreter::isIdentifierCharacter(char *character) {
    return (*character >= 'a' && *character <= 'z') || (*character >= 'A' && *character <= 'Z') || *character == '_';
}

bool RuMInterpreter::isDigit(char *character) {
    return *character >= '0' && *character <= '9';
}

void RuMInterpreter::ignoreComment() {
    if (*currentCharacter == '%') {
        while (*currentCharacter != '\n') {
            ++currentCharacter;
        }
        // Move to the next actual input on the next line
        getNextNonWhitespace();
    }
}

void RuMInterpreter::fillInputBuffer() {
    char *inputPosition = inputBuffer;
    bool endOfInputReached = false;
    do {
        try {
            std::cin.getline(inputPosition, MAX_INPUT_SIZE);
        }
        catch (std::exception e) {
            std::cout << "You have exceeded the input buffer size." << std::endl;
        }
        while (*inputPosition != NULL_CHAR) {
            ++inputPosition;
        }
        // We check to make sure that the input hasn't ended with a dollar sign
        if (*(inputPosition - 1) == this->END_CHAR) {
            endOfInputReached = true;
        }
        else {
            // Add in the \n that was missed by cin
            *inputPosition = '\n';
            ++inputPosition;
            if (isInteractiveMode) {
                std::cout << INTERPRETER_PROMPT_CONTINUED << " " << std::flush;
            }
        }
    } while (!endOfInputReached);
}

void RuMInterpreter::tokenize() {
    bool endOfInputReached = false;
    std::cout << "End of input reached" << std::endl;
    try {
        while (!endOfInputReached) {
            // Process a single token at a time
            getNextNonWhitespace();
            ignoreComment();
            if (isDigit(currentCharacter) || *currentCharacter == '.') {
                numericLiteral();
            }
            else if (*currentCharacter == '\"' || *currentCharacter == '\'') {
                stringLiteral();
            }
            else if (identifier());
            else if (specialToken());
            else if (*currentCharacter == END_CHAR) {
                Token token(std::string(1, END_CHAR), "endinput");
                this->tokenList.push_back(token);
                ++currentCharacter;
                endOfInputReached = true;
            }
            else {
                throw std::runtime_error("An invalid token was encountered");
            }
        }
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
    }
}

bool RuMInterpreter::numericLiteral() {
    std::string tokenCharacters;
    bool isFloat = false;
    while (isDigit(currentCharacter) || (*currentCharacter == '.' && !isFloat)) {
        if (*currentCharacter == '.') {
            isFloat = true;
        }
        tokenCharacters.push_back(*currentCharacter);
        ++currentCharacter;
    }
    Token token;
    token.setLexeme(tokenCharacters);
    if (isFloat) {
        if (tokenCharacters == ".") {
            // It's actually just a dot operator
            token.setTokenType("dot_op");
        }
        else {
            token.setTokenType("float");
        }
    }
    else {
        token.setTokenType("integer");
    }
    this->tokenList.push_back(token);
    return true;
}

void RuMInterpreter::stringLiteral() {
    std::string tokenCharacters;
    char stringDelimiter = *currentCharacter;
    tokenCharacters.push_back(*currentCharacter);
    ++currentCharacter;
    while (*currentCharacter != stringDelimiter && *currentCharacter != NULL_CHAR) {
        // Allow for escaped " or ' in the string
        if (*currentCharacter == '\\' && (*(currentCharacter + 1) == '"' || *(currentCharacter + 1) == '\'')) {
            tokenCharacters.push_back(*currentCharacter);
            ++currentCharacter;
        }
        tokenCharacters.push_back(*currentCharacter);
        ++currentCharacter;
    }
    if (*currentCharacter == NULL_CHAR) {
        throw std::runtime_error("End of input encountered inside string literal");
    }
    tokenCharacters.push_back(*currentCharacter);
    ++currentCharacter;
    Token token(tokenCharacters, "string");
    this->tokenList.push_back(token);
}

bool RuMInterpreter::specialToken() {
    std::string tokenCharacters;

    // First check to see if the first one or two characters are an operator
    tokenCharacters.push_back(*currentCharacter);
    ++currentCharacter;
    // Special case for when we are trying to catch a "=="
    if (tokenCharacters == "=" && *currentCharacter == '=') {
        tokenCharacters.push_back(*currentCharacter);
        ++currentCharacter;
    }
    if (operatorToken(tokenCharacters)) {
        return true;
    }
    else if (tokenCharacters == ";") {
        Token token(tokenCharacters, "semicolon");
        this->tokenList.push_back(token);
        return true;
    }
    tokenCharacters.push_back(*currentCharacter);
    if (operatorToken(tokenCharacters)) {
        return true;
    }
    // It wasn't a special character so we will need to back up to undo the move forward that we made
    currentCharacter -= 1;
    return false;
}

bool RuMInterpreter::operatorToken(std::string tokenCharacters) {
    if (operatorMap.find(tokenCharacters) != operatorMap.end()) {
        Token token(tokenCharacters, operatorMap[tokenCharacters]);
        this->tokenList.push_back(token);
        return true;
    }
    else {
        return false;
    }
}

bool RuMInterpreter::identifier() {
    std::string tokenCharacters;
    while (!isWhiteSpace(currentCharacter) && isIdentifierCharacter(currentCharacter)) {
        tokenCharacters.push_back(*currentCharacter);
        ++currentCharacter;
    }
    if (tokenCharacters.size() == 0) {
        return false;
    }
    // First check to see if it's a keyword
    if (keywordToken(tokenCharacters)) {
        return true;
    }
    else {
        Token token(tokenCharacters, "identifier");
        this->tokenList.push_back(token);
        return true;
    }
}

bool RuMInterpreter::keywordToken(std::string tokenCharacters) {
    if (keywordMap.find(tokenCharacters) != keywordMap.end()) {
        Token token(tokenCharacters, keywordMap[tokenCharacters]);
        this->tokenList.push_back(token);
        return true;
    }
    else {
        return false;
    }
}

void RuMInterpreter::interactiveMode() {
    this->isInteractiveMode = true;
    bool continueInterpreting = true;
    std::cout << "Welcome to the RuM interpreter!" << std::endl;
    std::cout << "You may enter any series of RuM commands here." << std::endl;
    std::cout << "It will run when the last character is a '" << this->END_CHAR << "'" << std::endl;
    std::cout << "'exit" << END_CHAR << "' to quit" << std::endl;
    while (continueInterpreting) {
        try {
            // Zeroing and resetting to make sure every run is clean.
            for (int i = 0; i < MAX_INPUT_SIZE; ++i) {
                inputBuffer[i] = NULL_CHAR;
            }
            // Empty out the token list
            tokenList = std::vector<Token>();
            currentCharacter = inputBuffer;
            std::cout << INTERPRETER_PROMPT_NEW << " " << std::flush;
            fillInputBuffer();
            tokenize();
            for (; tokenListPosition < tokenList.size(); ++tokenListPosition) {
                std::cout << "Token encountered: " << tokenList[tokenListPosition].getLexeme();
                std::cout << " . Category: " << tokenList[tokenListPosition].getTokenType() << std::endl;
            }
            currentCharacter = inputBuffer;
            // Parse the input
            parse();
            // Check to see if they have asked to exit
            if (tokenList.at(tokenList.size() - 2).getTokenType() == "exit_token") {
                std::cout << "Goodbye!" << std::endl;
                break;
            }
        }
        catch (const char *e) {
            std::cout << e << std::endl;
        }

    }
}

void RuMInterpreter::fileMode(char *filename) {
    std::ifstream in(filename);
    std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    std::copy(contents.begin(), contents.end(), inputBuffer);
    char *inputPosition = inputBuffer;
    while (*inputPosition != NULL_CHAR) {
        ++inputPosition;
    }
    --inputPosition;
    while (isWhiteSpace(inputPosition)) {
        --inputPosition;
    }
    if (*inputPosition != END_CHAR) {
        std::cout << inputBuffer << std::endl;
        std::cout << "A RuM file must end with a '" << END_CHAR << "'." << std::endl;
        return;
    }
    else {
        tokenize();
        for (; tokenListPosition < tokenList.size(); ++tokenListPosition) {
            std::cout << "Token encountered: " << tokenList[tokenListPosition].getLexeme();
            std::cout << " . Category: " << tokenList[tokenListPosition].getTokenType() << std::endl;
        }
        parse();
    }
}

void RuMInterpreter::parse() {
    try {
        this->parser.parseProgram();
    }
    catch (std::runtime_error e) {
        std::cout << e.what() << std::endl;
        parser.reset();
    }
}


