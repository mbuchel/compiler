//
// Created by Michael Buchel on 27/07/16.
//

#ifndef COMPILER_LEX_H
#define COMPILER_LEX_H

//Constants
#define EoF 256
#define DIGIT 257

//Debug
#define LEX_DEBUG

//Token for each Expression
struct tokenType{
    int classSymbol;
    char representation;
};

//Expression struct
//This is also the nodeAST
struct Expression {
    char type;
    int value, opr;
    Expression *left, *right;
};

//Declares Token for ease of use and getNextToken
extern tokenType Token;
extern void getNextToken();

#endif //COMPILER_LEX_H
