//
// Created by Michael Buchel on 18/07/16.
//

#include <iostream>
#include "VM.h"

//Loads the code into the program
void VM::load(uint64_t *code) {
    int i;

    for (i = 0; code[i] != EXIT_CODE; ++i) {
        codeMemory[i] = code[i];
    }

    codeMemory[i] = EXIT_CODE;
}

//Running the program
void VM::run() {
    int i = 0;
    uint8_t opCode;
    uint64_t temp;

    //The actual code
    while (codeMemory[i] != EXIT_CODE) {
        //Masks the code so we only get the top 8 bits (as all op-codes are a byte long)
        opCode = (uint8_t) (codeMemory[i] & OPCODE_BITMASK) >> 56; //Moves the op-code to the bottom
        switch (opCode) {
            case MOV:
                //Hides the op-code
                temp = codeMemory[i] & OPCODE_BITMASK_HIDE;

                EDI = (uint16_t) ((temp >> 24) & 0xFFFF); //Parses the destination location
                ESI = (uint16_t) ((temp >> 8) & 0xFFFF); //Parses the source location

                temp = stackMemory[ESI / 8]; //Pulls the data

                DS = (uint16_t) ((temp >> (64 - ((ESI % 8) + 1) * 8)) & 0xFF); //Actual data for transfer

                temp = 0xFF00000000000000; //Start bitmask

                //Moves FF to the correct position
                for (int j = 0; j < EDI % 8; j++)
                    temp = temp >> 16;

                //Sets the memory area we are writing to to zero
                stackMemory[EDI / 8] = stackMemory[EDI / 8] & ~temp;

                //Moves data to start location
                temp = DS << 56;

                //Moves data to correct location
                for (int j = 0; j < EDI % 8; j++)
                    temp = temp >> 16;

                //Writes data to location
                stackMemory[EDI / 8] = stackMemory[EDI / 8] | temp;
                break;
            case PUSH:
            case POP:
            case LEA:
            case ADD:
            case SUB:
            case INC:
            case DEC:
            case IMUL:
            case IDIV:
            case AND:
            case OR:
            case XOR:
            case NOT:
            case NEG:
            case SHL:
            case SHR:
            case JMP:
            case JE:
            case JNE:
            case JZ:
            case JNZ:
            case JG:
            case JGE:
            case JL:
            case JLE:
            case CMP:
            case CALL:
            case RET:
            default:
                break;
        }
        ++i;
    }
}