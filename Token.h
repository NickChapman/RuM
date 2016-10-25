//
// Created by Nick Chapman on 10/15/16.
//

#ifndef PROJECT2_TOKEN_H
#define PROJECT2_TOKEN_H


#include <string>

class Token {
private:
    std::string tokenType;
    std::string lexeme;
public:
    Token();

    Token(std::string lexeme, std::string tokenType);

    Token(const Token &toCopy);

    std::string getTokenType();

    void setTokenType(std::string tokenType);

    std::string getLexeme();

    void setLexeme(std::string lexeme);
};


#endif //PROJECT2_TOKEN_H
