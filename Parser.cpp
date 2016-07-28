//
// Created by Michael Buchel on 24/07/16.
//

#include <iostream>
#include "Parser.h"
#include "Error.h" //For error handling

//Checks the type of operator
bool parser::parseOp(int *op) {
    //Switches to see if it is an operator
    switch (Token.classSymbol) {
        case '+':
        case '*':
        case '-':
        case '/':
        case '%':
            *op = Token.classSymbol;
            getNextToken();
            return true;
        default:
            return false;
    }
}

//Parses the expression
//The pointer pointer is for the tree structure
bool parser::parseExp(Expression **pExpression) {
#ifdef PARSE_DEBUG
    std::cout << "expression parsed\n";
#endif
    Expression *exp = *pExpression = new Expression;

    //If the leaf is a digit
    if (Token.classSymbol == DIGIT) {
        exp->type = 'D';
        exp->value = Token.representation - '0';
        getNextToken();
        parseOp(&exp->opr);
        return true;
    }

    //Code for brackets
    if (Token.classSymbol == '(') {
        exp->type = 'P';
        getNextToken();
        //Looks for second bracket
        if (!parseExp(&exp->left)) {
            Error::error("Missing expression");
        }
        if (!parseOp(&exp->opr)) {
            Error::error("Missing operator");
        } else {
            int temp = exp->opr;
            std::cout << temp << std::endl;
        }
        if (!parseExp(&exp->right)) {
            Error::error("Missing expression");
        }
        getNextToken();
        return true;
    }

    //In case it is not a digit or operator or anything
    delete exp;
    return false;
}

bool parser::parse(nodeAST **input) {
    Expression *expression;

    //Starts lexial analysis
    do {
        getNextToken();
        if (parseExp(&expression)) {
            /*if (Token.classSymbol != EoF) {
                Error::error("Garbage after program");
            }*/
#ifdef PARSE_DEBUG
            std::cout << expression->opr << " " << expression->right << " " << expression->left << " "
                      << expression->type << " " << expression->value << std::endl;
#endif
            *input = expression;
            return true;
        }
    } while (Token.classSymbol != EoF);


    return false;
}