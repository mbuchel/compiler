//
// Created by Michael Buchel on 21/07/16.
//

//TODO: delete this class

#include <iostream>
#include "LexialReader.h"

LexialReader::LexialReader() {
    //Clears program
    for (int i = 0; i < PROGRAM_LENGTH; ++i)
        program[i] = 0;
}

//Set file name
void LexialReader::setFile(std::string name) {
    fileName = name;
}

//Reads the file
void LexialReader::readFile() {
    uint64_t temp; //Temp for parsing the lexemes
    //Opens file
    myfile.open(fileName);

    //Sets the program counter = 0
    programCounter = 0;

    //If no errors in opening the file
    if (myfile.is_open()) {
        //While loop to go through the file
        while (getline(myfile,line)) {

            //Because there is no out of bound array checking in C++ so I am setting those array elements to zero
            if (line.length() < 8) {
                for (int j = (int) line.length(); j < 8; ++j) {
                    line[j] = 0;
                }
            }

            //Because the loop was not working
            temp = (uint64_t) line[0] << 56;
            temp = (uint64_t) temp | ((uint64_t) line[1] << 48);
            temp = (uint64_t) temp | ((uint64_t) line[2] << 40);
            temp = (uint64_t) temp | ((uint64_t) line[3] << 32);
            temp = (uint64_t) temp | ((uint64_t) line[4] << 24);
            temp = (uint64_t) temp | ((uint64_t) line[5] << 16);
            temp = (uint64_t) temp | ((uint64_t) line[6] << 8);
            temp = (uint64_t) temp | line[7];

            program[programCounter++] = temp;
        }
        myfile.close();
    } else {
        //If file does not open
        std::cout << "ERROR\n\n";
    }

#ifdef DEBUG
    for (int j = 0; j < 5; ++j) {
        std::cout << std::hex << program[j] << std::endl;
    }
#endif
}