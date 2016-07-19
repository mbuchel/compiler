//
// Created by Michael Buchel on 18/07/16.
//

#include "VM.h"

//Loads the code into the program
void VM::load(uint64_t *code) {
    int i;

    for (i = 0; code[i] != EXIT_CODE; ++i) {
        codeMemory[i] = code[i];
    }

    codeMemory[i] = EXIT_CODE;

    for (i = 0; i < STACK_SPACE; ++i) {
        stackMemory[i] = 0;
        metadataStack[i] = 0;
    }

    for (i = 0; i < HEAP_SPACE; ++i) {
        heapMemory[i] = 0;
        metadataHeap[i] = 0;
    }
}

//Running the program
void VM::run() {
    int i = 0;
    uint8_t opCode, type;
    uint64_t temp;

    //The actual code
    while (codeMemory[i] != EXIT_CODE) {
        //Masks the code so we only get the top 8 bits (as all op-codes are a byte long)
        opCode = (uint8_t) (codeMemory[i] & OPCODE_BITMASK) >> 56; //Moves the op-code to the bottom
        switch (opCode) {
            case MOV:
                //Checks type of move
                type = (uint8_t) (codeMemory[i] & INSTRUCTION_GET_MASK);

                //Hides the op-code
                temp = codeMemory[i] & OPCODE_BITMASK_HIDE;

                EDI = (uint16_t) ((temp >> 24) & 0xFFFF); //Parses the destination location
                ESI = (uint16_t) ((temp >> 8) & 0xFFFF); //Parses the source location

                //Different types of the instruction have different algorithms
                switch(type) {
                    //If it is a register to register move
                    case REG_REG:
                        //There are a lot of read and write registers function calls so this is just for
                        readReg(temp);
                        writeReg(temp);
                        break;
                    case REG_LIT:
                    case REG_MEM:
                    case MEM_REG:
                    case MEM_LIT:
                    case MEM_MEM:
                        temp = stackMemory[ESI / 8]; //Pulls the data

                        DS = (uint16_t) ((temp >> (64 - ((ESI % 8) + 1))) & 0xFF); //Actual data for transfer

                        temp = 0xFF00000000000000; //Start bitmask

                        //Move FF to the correct position
                        for (int j = 0; j < EDI % 8; ++j)
                            temp = temp >> 8;

                        //Sets the memory area we are writing to to zero
                        stackMemory[EDI / 8] = stackMemory[EDI / 8] & ~temp;

                        //Moves data to start location
                        temp = DS << 56;

                        //Moves data to correct location
                        for (int j = 0; j < EDI / 8; ++j)
                            temp = temp >> 8;

                        //Writes to location
                        stackMemory[EDI / 8] = stackMemory[EDI / 8] | temp;
                        break;
                    default:
                        break;
                }
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

//Function to read from a register
void VM::readReg(uint64_t &temp) {
    //Move the source into the temp variable
    /*
     * Originally planned to make this a union and switch based on the address
     * The issue with this is it will take the same amount of checks and possibly more
     * So I decided to just do it with switch statements
     */
    switch (ESI) {
        case EAX_ADDRESS:
            temp = EAX;
            return;
        case EBX_ADDRESS:
            temp = EBX;
            return;
        case ECX_ADDRESS:
            temp = ECX;
            return;
        case EDX_ADDRESS:
            temp = EDX;
            return;
        case AX_ADDRESS:
            temp = AX;
            return;
        case BX_ADDRESS:
            temp = BX;
            return;
        case CX_ADDRESS:
            temp = CX;
            return;
        case DX_ADDRESS:
            temp = DX;
            return;
        case AH_ADDRESS:
            temp = AH;
            return;
        case AL_ADDRESS:
            temp = AL;
            return;
        case BH_ADDRESS:
            temp = BH;
            return;
        case BL_ADDRESS:
            temp = BL;
            return;
        case CH_ADDRESS:
            temp = CH;
            return;
        case CL_ADDRESS:
            temp = CL;
            return;
        case DH_ADDRESS:
            temp = DH;
            return;
        case DL_ADDRESS:
            temp = DL;
            return;
        case CS_ADDRESS:
            temp = CS;
            return;
        case DS_ADDRESS:
            temp = DS;
            return;
        case ES_ADDRESS:
            temp = ES;
            return;
        case FS_ADDRESS:
            temp = FS;
            return;
        case GS_ADDRESS:
            temp = GS;
            return;
        case SS_ADDRESS:
            temp = SS;
            return;
        case EDI_ADDRESS:
            temp = EDI;
            return;
        case ESI_ADDRESS:
            temp = ESI;
            return;
        case ESP_ADDRESS:
            temp = ESP;
            return;
        case EIP_ADDRESS:
            temp = EIP;
            return;
        case EFLAGS_ADDRESS:
            temp = EFLAGS;
            return;
        default:
            return;
    }
}

//Function to write into a register
void VM::writeReg(uint64_t &temp) {
    //Temp variable into destination address
    switch (EDI) {
        case EAX_ADDRESS:
            EAX = (uint32_t) temp;
            return;
        case EBX_ADDRESS:
            EBX = (uint32_t) temp;
            return;
        case ECX_ADDRESS:
            ECX = (uint32_t) temp;
            return;
        case EDX_ADDRESS:
            EDX = (uint32_t) temp;
            return;
        case AX_ADDRESS:
            AX = (uint16_t) temp;
            return;
        case BX_ADDRESS:
            BX = (uint16_t) temp;
            return;
        case CX_ADDRESS:
            CX = (uint16_t) temp;
            return;
        case DX_ADDRESS:
            DX = (uint16_t) temp;
            return;
        case AH_ADDRESS:
            AH = (uint8_t) temp;
            return;
        case AL_ADDRESS:
            AL = (uint8_t) temp;
            return;
        case BH_ADDRESS:
            BH = (uint8_t) temp;
            return;
        case BL_ADDRESS:
            BL = (uint8_t) temp;
            return;
        case CH_ADDRESS:
            CH = (uint8_t) temp;
            return;
        case CL_ADDRESS:
            CL = (uint8_t) temp;
            return;
        case DH_ADDRESS:
            DH = (uint8_t) temp;
            return;
        case DL_ADDRESS:
            DL = (uint8_t) temp;
            return;
        case CS_ADDRESS:
            CS = (uint16_t) temp;
            return;
        case DS_ADDRESS:
            DS = (uint16_t) temp;
            return;
        case ES_ADDRESS:
            ES = (uint16_t) temp;
            return;
        case FS_ADDRESS:
            FS = (uint16_t) temp;
            return;
        case GS_ADDRESS:
            GS = (uint16_t) temp;
            return;
        case SS_ADDRESS:
            SS = (uint16_t) temp;
            return;
        case EDI_ADDRESS:
            EDI = (uint16_t) temp;
            return;
        case ESI_ADDRESS:
            ESI = (uint16_t) temp;
            return;
        case ESP_ADDRESS:
            ESP = (uint16_t) temp;
            return;
        case EIP_ADDRESS:
            EIP = (uint16_t) temp;
            return;
        case EFLAGS_ADDRESS:
            EFLAGS = (uint32_t) temp;
        default:
            return;
    }
}