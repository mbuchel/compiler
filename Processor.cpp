//
// Created by Michael Buchel on 27/07/16.
//

#include <iostream>

#include "Processor.h"
#include "Error.h"

//Runs the code
void Processor::process(nodeAST *code) {
    codeGen(code);
    std::cout << "\tPRINT\n";
}

//Generates the code
void Processor::codeGen(Expression *code) {
#ifdef PARSE_DEBUG
    std::cout << "CODE BEING GENERATED\n";
#endif
    switch (code->type) {
        case 'D':
        std::cout << "\tPUSH " << code->value << std::endl;
            break;
        case 'P':
            //Checks the sides of the tree first before adding other code
            codeGen(code->left);
            codeGen(code->right);
            switch (code->opr) {
                case '+':
                std::cout << "\tADD\n";
                    break;
                case '*':
                std::cout << "\tMULT\n";
                    break;
                case '-':
                std::cout << "\tSUB\n";
                    break;
                case '/':
                std::cout << "\tDIV\n";
                    break;
                case '%':
                std::cout << "\tREM\n";
                    break;
                default: //Default case just in case
                    break;
            }
            break;
        default:
        Error::error("Error in AST");
            break;
    }
}