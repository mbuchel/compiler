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

void VM::run() {
    int i = 0;
    uint64_t opCode;

    while (codeMemory[i] != EXIT_CODE) {
        opCode = codeMemory[i] | OPCODE_BITMASK;
        opCode = opCode >> 56;
        switch (opCode) {
            case MOV:
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
                std::cout << opCode << std::endl;
                break;
        }
        ++i;
    }
}