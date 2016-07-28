//
// Created by Michael Buchel on 27/07/16.
//

#ifndef COMPILER_PROCESSOR_H
#define COMPILER_PROCESSOR_H


#include "Parser.h"

//Debug include
#define PARSE_DEBUG

//Class to run the code currently turns this into a simply asm code
namespace Processor {
    //Runs the code
    void process(nodeAST *code);

    //Generates it
    void codeGen(Expression *code);
}


#endif //COMPILER_PROCESSOR_H
