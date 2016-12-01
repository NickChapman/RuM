#include <iostream>
#include "RuM.h"

/*
 * DISCLAIMERS:
 * File reading code was taken from here: http://stackoverflow.com/questions/18398167/how-to-copy-a-txt-file-to-a-char-array-in-c
 * and from here: http://stackoverflow.com/questions/347949/how-to-convert-a-stdstring-to-const-char-or-char
 *
 * Additionally, some inspiration was taken from the sample project code provided
 */

int main(int argc, char* argv[]) {
    RuMInterpreter interpreter;
    interpreter.setDisplayParseTree(false);
    if (argc == 2) {
        // Pass the filename to the interpreter
        try {
            std::cout << "Running file: " << argv[1] << std::endl;
            interpreter.fileMode(argv[1]);
        }
        catch (const std::runtime_error& e) {
            std::cout << e.what() << std::endl;
        }
    }
    interpreter.interactiveMode();
    return 0;
}