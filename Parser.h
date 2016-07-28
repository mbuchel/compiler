//
// Created by Michael Buchel on 24/07/16.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "lex.h"

//For debugging
#define PARSE_DEBUG

//AST struct
typedef Expression nodeAST;

//For parsing the class namespace for easy expansion
namespace parser {
    //Parsing the operator
    bool parseOp(int *op);
    //Parsing the Expression
    bool parseExp(Expression **pExpression);
    //Master parse start
    bool parse(nodeAST **input);
}


#endif //COMPILER_PARSER_H
