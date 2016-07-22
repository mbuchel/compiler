//
// Created by Michael Buchel on 21/07/16.
//

//TODO: delete this class

#include <fstream>

#ifndef COMPILER_LEXIALREADER_H
#define COMPILER_LEXIALREADER_H

//#define DEBUG

#define PROGRAM_LENGTH 3000

class LexialReader {
private:
    int programCounter;
    uint64_t program[PROGRAM_LENGTH]; //Program size of 24 kilobytes
    std::string line, fileName;
    std::ifstream myfile;
public:
    LexialReader();
    void setFile(std::string name);
    void readFile();
};


#endif //COMPILER_LEXIALREADER_H