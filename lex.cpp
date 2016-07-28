//
// Created by Michael Buchel on 27/07/16.
//

#include <cstdio>
#include <iostream>

#include "lex.h"

//Checks if it is a layout char (so whitespace)
static bool layoutChar(int input) {
    switch (input) {
        case ' ':
        case '\t':
        case '\n':
            return true;
        default:
            return false;
    }
}

tokenType Token;

//Gets the next token and says it is a certain type
void getNextToken() {
    int futureInput;

    do {
        futureInput = std::getchar();
        if (futureInput < 0) {
            Token.classSymbol = EoF;
            Token.representation = '#';
            return;
        }
    } while (layoutChar(futureInput));

    //Checks the type of input it is
    if ('0' <= futureInput && futureInput <= '9') {
        Token.classSymbol = DIGIT;
    } else {
        Token.classSymbol = futureInput;
    }

    Token.representation = (char) futureInput;

#ifdef LEX_DEBUG
    std::cout << Token.classSymbol << ", " << Token.representation << std::endl;
#endif
}