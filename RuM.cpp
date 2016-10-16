//
// Created by Nick Chapman on 10/15/16.
//

#include "RuM.h"

RuMInterpreter::RuMInterpreter() {
    for (int i = 0; i < MAX_INPUT_SIZE; ++i) {
        inputBuffer[i] = NULL;
    }
    for (int i = 0; i < MAX_OUTPUT_SIZE; ++i) {
        outputBuffer[i] = NULL;
    }
}

void RuMInterpreter::getNextNonWhitespace() {
    while (isWhiteSpace(currentCharacter)) {
        ++currentCharacter;
    }
}

bool RuMInterpreter::isWhiteSpace(char *character) {
    return (*character == ' ' || *character == '\n' || *character == '\r' || *character == '\t');
}

void RuMInterpreter::ignoreComment() {
    if (*currentCharacter == '%') {
        while (*currentCharacter != '\n') {
            ++currentCharacter;
        }
        // Increment one more time to move to the next line
        ++currentCharacter;
    }
}

void RuMInterpreter::tokenize() {
    bool endOfInputReached = false;
    while (!endOfInputReached) {
        fillInputBuffer();
        if (*currentCharacter == '$'){
            endOfInputReached = true;
        }
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
        while (*inputPosition != NULL) {
            ++inputPosition;
        }
        // We check to make sure that the input hasn't ended with a dollar sign
        if (*(inputPosition - 1) == '$') {
            endOfInputReached = true;
        }
    } while (!endOfInputReached);
}
