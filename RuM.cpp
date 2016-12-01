//
// Created by Nick Chapman on 10/15/16.
//

#include "RuM.h"
#include <fstream>
#include <sstream>

#define NULL_CHAR '\0'

RuMInterpreter::RuMInterpreter() {
    // Setting constant values
    this->INTERPRETER_PROMPT_NEW = "RuM>";
    this->INTERPRETER_PROMPT_CONTINUED = ">>>";

    // Initializing other interpreter values
    this->isInteractiveMode = false;
    inputBuffer = std::string();
    currentCharacterIndex = 0;
    tokenListPosition = 0;
    tokenList = std::make_shared<std::vector<Token>>();

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
    this->parser = RuMParser(tokenList);
}

const char &RuMInterpreter::currentCharacter() const {
    return this->inputBuffer.at(currentCharacterIndex);
}


void RuMInterpreter::getNextNonWhitespace() {
    while (isWhiteSpace(currentCharacter())) {
        ++currentCharacterIndex;
    }
}

bool RuMInterpreter::isWhiteSpace(const char &character) {
    return (character == ' ' || character == '\n' || character == '\r' || character == '\t');
}

bool RuMInterpreter::isIdentifierCharacter(const char &character) {
    return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || character == '_';
}

bool RuMInterpreter::isDigit(const char &character) {
    return character >= '0' && character <= '9';
}

void RuMInterpreter::ignoreComment() {
    if (currentCharacter() == '%') {
        while (currentCharacter() != '\n' && currentCharacter() != '\r') {
            ++currentCharacterIndex;
        }
        // Move to the next actual input on the next line
        getNextNonWhitespace();
    }
}

void RuMInterpreter::fillInputBuffer() {
    bool endOfInputReached = false;
    do {
        try {
            char *inputCharBuffer = new char[MAX_INPUT_SIZE];
            std::cin.getline(inputCharBuffer, MAX_INPUT_SIZE);
            inputBuffer += inputCharBuffer;
            delete[] inputCharBuffer;
        }
        catch (const std::runtime_error e) {
            std::cout << e.what() << std::endl;
        }
        // We check to make sure that the input hasn't ended with a dollar sign
        if (inputBuffer == "") {
            std::cout << INTERPRETER_PROMPT_NEW << " " << std::flush;
        }
        else if (inputBuffer.at(inputBuffer.size() - 1) == this->END_CHAR) {
            endOfInputReached = true;
        }
        else {
            // Add in the \n that was missed by cin
            inputBuffer += '\n';
            if (isInteractiveMode) {
                std::cout << INTERPRETER_PROMPT_CONTINUED << " " << std::flush;
            }
        }
    } while (!endOfInputReached);
}

void RuMInterpreter::tokenize() {
    bool endOfInputReached = false;
    try {
        while (!endOfInputReached) {
            // Process a single token at a time
            getNextNonWhitespace();
            ignoreComment();
            if (isDigit(currentCharacter()) || currentCharacter() == '.') {
                numericLiteral();
            }
            else if (currentCharacter() == '\"' || currentCharacter() == '\'') {
                stringLiteral();
            }
            else if (identifier());
            else if (specialToken());
            else if (currentCharacter() == END_CHAR) {
                Token token(std::string(1, END_CHAR), "endinput");
                this->tokenList->push_back(token);
                ++currentCharacterIndex;
                endOfInputReached = true;
            }
            else {
                throw std::runtime_error("An invalid token was encountered");
            }
        }
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

bool RuMInterpreter::numericLiteral() {
    std::string tokenCharacters;
    bool isFloat = false;
    while (isDigit(currentCharacter()) || (currentCharacter() == '.' && !isFloat)) {
        if (currentCharacter() == '.') {
            isFloat = true;
        }
        tokenCharacters.push_back(currentCharacter());
        ++currentCharacterIndex;

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
    this->tokenList->push_back(token);
    return true;
}

void RuMInterpreter::stringLiteral() {
    std::string tokenCharacters;
    char stringDelimiter = currentCharacter();
    tokenCharacters.push_back(currentCharacter());
    ++currentCharacterIndex;
    while (currentCharacter() != stringDelimiter && currentCharacter() != NULL_CHAR) {
        // Allow for escaped " or ' in the string
        if (currentCharacter() == '\\' &&
            (inputBuffer.at(currentCharacterIndex + 1) == '"' || inputBuffer.at(currentCharacterIndex + 1) == '\'')) {
            ++currentCharacterIndex;
            tokenCharacters.push_back(currentCharacter());
            ++currentCharacterIndex;
        }
        tokenCharacters.push_back(currentCharacter());
        ++currentCharacterIndex;
    }
    if (currentCharacter() == NULL_CHAR) {
        throw std::runtime_error("End of input encountered inside string literal");
    }
    tokenCharacters.push_back(currentCharacter());
    ++currentCharacterIndex;
    Token token(tokenCharacters, "string");
    this->tokenList->push_back(token);
}

bool RuMInterpreter::specialToken() {
    std::string tokenCharacters;

    // First check to see if the first one or two characters are an operator
    tokenCharacters.push_back(currentCharacter());
    ++currentCharacterIndex;
    // Special case for when we are trying to catch a "=="
    if (tokenCharacters == "=" && currentCharacter() == '=') {
        tokenCharacters.push_back(currentCharacter());
        ++currentCharacterIndex;
    }
    // Special case for when we are trying to catch a "&&"
    if(tokenCharacters == "&" && currentCharacter() == '&') {
        tokenCharacters.push_back(currentCharacter());
        ++currentCharacterIndex;
    }
    if (operatorToken(tokenCharacters)) {
        return true;
    }
    else if (tokenCharacters == ";") {
        Token token(tokenCharacters, "semicolon");
        this->tokenList->push_back(token);
        return true;
    }
    // We have checked the one character tokens. Let's check the two character tokens
    if (currentCharacterIndex != inputBuffer.size()) {
        tokenCharacters.push_back(currentCharacter());
        ++currentCharacterIndex;
        if (operatorToken(tokenCharacters)) {
            return true;
        }
        else {
            --currentCharacterIndex;
        }
    }
    // It wasn't a special character so we will need to back up to undo the move forward that we made
    --currentCharacterIndex;
    return false;
}

bool RuMInterpreter::operatorToken(std::string tokenCharacters) {
    if (operatorMap.find(tokenCharacters) != operatorMap.end()) {
        Token token(tokenCharacters, operatorMap[tokenCharacters]);
        this->tokenList->push_back(token);
        return true;
    }
    else {
        return false;
    }
}

bool RuMInterpreter::identifier() {
    std::string tokenCharacters;
    while (!isWhiteSpace(currentCharacter()) && isIdentifierCharacter(currentCharacter())) {
        tokenCharacters.push_back(currentCharacter());
        ++currentCharacterIndex;
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
        this->tokenList->push_back(token);
        return true;
    }
}

bool RuMInterpreter::keywordToken(std::string tokenCharacters) {
    if (keywordMap.find(tokenCharacters) != keywordMap.end()) {
        Token token(tokenCharacters, keywordMap[tokenCharacters]);
        this->tokenList->push_back(token);
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
            // Empty out the token list
            tokenList->clear();
            // Clean up the input buffer
            inputBuffer = "";
            currentCharacterIndex = 0;
            std::cout << INTERPRETER_PROMPT_NEW << " " << std::flush;
            fillInputBuffer();
            tokenize();
            if (displayTokenization) {
                for (; tokenListPosition < tokenList->size() - 1; ++tokenListPosition) {
                    std::cout << "Token encountered: " << tokenList->at(tokenListPosition).getLexeme();
                    std::cout << " . Category: " << tokenList->at(tokenListPosition).getTokenType() << std::endl;
                }
            }
            currentCharacterIndex = 0;
            // Check to see if they have asked to exit
            if (tokenList->size() < 2) {
                // They didn't do anything
                continue;
            }
            if (tokenList->at(tokenList->size() - 2).getTokenType() == "exit_token") {
                std::cout << "Goodbye!" << std::endl;
                break;
            }
            // Parse the input
            parse();
        }
        catch (const std::runtime_error &e) {
            std::cout << e.what() << std::endl;
        }
        catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }
}

void RuMInterpreter::fileMode(char *filename) {
    std::ifstream in(filename);
    std::stringstream buffer;
    buffer << in.rdbuf();
    inputBuffer = buffer.str();
    unsigned long inputPosition = inputBuffer.size() - 1;
    while (isWhiteSpace(inputBuffer.at(inputPosition))) {
        --inputPosition;
    }
    if (inputBuffer.at(inputPosition) != END_CHAR) {
        std::cout << inputBuffer << std::endl;
        std::cout << "A RuM file must end with a '" << END_CHAR << "'." << std::endl;
        return;
    }
    else {
        tokenize();
        if(this->displayTokenization) {
            for (; tokenListPosition < tokenList->size(); ++tokenListPosition) {
                std::cout << "Token encountered: " << tokenList->at(tokenListPosition).getLexeme();
                std::cout << " . Category: " << tokenList->at(tokenListPosition).getTokenType() << std::endl;
            }
        }
        parse();
    }
}

void RuMInterpreter::setDisplayParseTree(bool display) {
    this->parser.setDisplayParseTree(display);
}

void RuMInterpreter::setDisplayTokenization(bool display) {
    this->displayTokenization = true;
}

void RuMInterpreter::parse() {
    try {
        this->parser.parseProgram();
    }
    catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        parser.reset();
    }
}


