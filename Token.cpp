//
// Created by Nick Chapman on 10/15/16.
//

#include "Token.h"

Token::Token() {
    this->lexeme = "";
    this->tokenType = "";
}

Token::Token(std::string lexeme, std::string tokenType) {
    this->lexeme = lexeme;
    this->tokenType = tokenType;
}

Token::Token(const Token &toCopy) {
    this->lexeme = toCopy.lexeme;
    this->tokenType = toCopy.tokenType;
}

std::string Token::getTokenType() {
    return this->tokenType;
}

void Token::setTokenType(std::string tokenType) {
    this->tokenType = tokenType;
}

std::string Token::getLexeme() {
    return this->lexeme;
}

void Token::setLexeme(std::string lexeme) {
    this->lexeme = lexeme;
}

