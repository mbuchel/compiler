//Using this for testing purposes

#include "Parser.h" //For parsing the data & making the AST
#include "Processor.h" //For processing the data
#include "Error.h" //For error checking

int main() {
    //Variables for AST
    nodeAST *code;

    //If error in code to say error
    //getNextToken();
    if (!parser::parse(&code))
        Error::error("No top level expression");

    //Run code
    Processor::process(code);

    return 0;
}