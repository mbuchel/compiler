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
    uint8_t bitSize;
    uint64_t temp, temp2;

    //The actual code
    while (codeMemory[i] != EXIT_CODE) {
        //Masks the code so we only get the top 8 bits (as all op-codes are a byte long)
        CS = (uint8_t) (codeMemory[i] & OPCODE_BITMASK) >> 56; //Moves the op-code to the bottom
        switch (CS) {
            case MOV:
                //Checks type of move
                ES = (uint8_t) (codeMemory[i] & INSTRUCTION_GET_MASK);

                //Hides the op-code
                temp = codeMemory[i] & OPCODE_BITMASK_HIDE;

                EDI = (uint16_t) ((temp >> 24) & 0xFFFF); //Parses the destination location
                ESI = (uint16_t) ((temp >> 8) & 0xFFFF); //Parses the source location

                //Different types of the instruction have different algorithms
                switch(ES) {
                    //If it is a register to register move
                    case REG_REG:
                        //There are a lot of read and write registers function calls so this is just for
                        readReg(bitSize, temp);
                        writeReg(temp);
                        break;
                    case REG_LIT:
                        //Because the source is the literal
                        temp = ESI;
                        writeReg(temp);
                        break;
                    case REG_MEM:
                        //Gets the type of data that is held
                        getMetaDataStack(bitSize);

                        //If 32 bits as the DS register is only 16 bits
                        if (bitSize == BIT32_SIZE) {
                            //Treats 32 bit data as 2 16 bit data
                            bitSize = BIT16_SIZE;

                            //Read from memory
                            readMem(bitSize);
                            //Move to temp register for writing
                            temp = DS;
                            temp = temp << 16;
                            //Writes to the register
                            writeReg(temp);

                            ESI = (uint16_t) (ESI + 0x10);

                            //Read from memory
                            readMem(bitSize);
                            //Move to temp register for writing
                            temp = temp | DS;
                            //Writes to the register
                            writeReg(temp);
                        } else {
                            //Reads from memory
                            readMem(bitSize);
                            //Moves into the temp register for writing
                            temp = DS;
                            //Writes to the register
                            writeReg(temp);
                        }
                        break;
                    case MEM_REG:
                        //Reads from register
                        readReg(bitSize, temp);

                        //Checks the size of the data
                        if (bitSize == BIT32_SIZE) {
                            //Writes to metadata
                            writeMetaDataStack(bitSize, temp2);

                            //To allow us to use the 16 bit code again
                            bitSize = 16;

                            DS = (uint16_t) (temp >> 16); //Sets first write register

                            //Writes upper word into memory
                            writeMem(bitSize, temp2);

                            DS = (uint16_t) (temp & 0xFFFF); //Sets second write register

                            //Writes lower word into memory after moving the location
                            ESI = (uint16_t) (ESI + 0x10);
                            writeMem(bitSize, temp2);
                        } else {
                            //Writes to metadata
                            writeMetaDataStack(bitSize, temp2);
                            DS = (uint16_t) temp; //Loads write register
                            writeMem(bitSize, temp); //Writes to memory
                        }
                        break;
                    case MEM_LIT:
                    case MEM_MEM:
                        //Reads memory
                        getMetaDataStack(bitSize);
                        readMem(bitSize);
                        //Writes to memory
                        writeMetaDataStack(bitSize, temp2);
                        writeMem(bitSize, temp);
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
        bitSize = 0;
    }
}

//Function to read from a register
void VM::readReg(uint8_t &data, uint64_t &temp) {
    //Move the source into the temp variable
    /*
     * Originally planned to make this a union and switch based on the address
     * The issue with this is it will take the same amount of checks and possibly more
     * So I decided to just do it with switch statements
     */
    switch (ESI) {
        case EAX_ADDRESS:
            temp = EAX;
            data = BIT32_SIZE;
            return;
        case EBX_ADDRESS:
            temp = EBX;
            data = BIT32_SIZE;
            return;
        case ECX_ADDRESS:
            temp = ECX;
            data = BIT32_SIZE;
            return;
        case EDX_ADDRESS:
            temp = EDX;
            data = BIT32_SIZE;
            return;
        case AX_ADDRESS:
            temp = AX;
            data = BIT16_SIZE;
            return;
        case BX_ADDRESS:
            temp = BX;
            data = BIT16_SIZE;
            return;
        case CX_ADDRESS:
            temp = CX;
            data = BIT16_SIZE;
            return;
        case DX_ADDRESS:
            temp = DX;
            data = BIT16_SIZE;
            return;
        case AH_ADDRESS:
            temp = AH;
            data = BIT8_SIZE;
            return;
        case AL_ADDRESS:
            temp = AL;
            data = BIT8_SIZE;
            return;
        case BH_ADDRESS:
            temp = BH;
            data = BIT8_SIZE;
            return;
        case BL_ADDRESS:
            temp = BL;
            data = BIT8_SIZE;
            return;
        case CH_ADDRESS:
            temp = CH;
            data = BIT8_SIZE;
            return;
        case CL_ADDRESS:
            temp = CL;
            data = BIT8_SIZE;
            return;
        case DH_ADDRESS:
            temp = DH;
            data = BIT8_SIZE;
            return;
        case DL_ADDRESS:
            temp = DL;
            data = BIT8_SIZE;
            return;
        case CS_ADDRESS:
            temp = CS;
            data = BIT16_SIZE;
            return;
        case DS_ADDRESS:
            temp = DS;
            data = BIT16_SIZE;
            return;
        case ES_ADDRESS:
            temp = ES;
            data = BIT16_SIZE;
            return;
        case FS_ADDRESS:
            temp = FS;
            data = BIT16_SIZE;
            return;
        case GS_ADDRESS:
            temp = GS;
            data = BIT16_SIZE;
            return;
        case SS_ADDRESS:
            temp = SS;
            data = BIT16_SIZE;
            return;
        case EDI_ADDRESS:
            temp = EDI;
            data = BIT16_SIZE;
            return;
        case ESI_ADDRESS:
            temp = ESI;
            data = BIT16_SIZE;
            return;
        case ESP_ADDRESS:
            temp = ESP;
            data = BIT16_SIZE;
            return;
        case EIP_ADDRESS:
            temp = EIP;
            data = BIT16_SIZE;
            return;
        case EFLAGS_ADDRESS:
            temp = EFLAGS;
            data = BIT32_SIZE;
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

//To read from memory
void VM::readMem(uint8_t &data) {
    switch (data) {
        case BIT8_SIZE:
            DS = (uint16_t) ((stackMemory[ESI / 64] >> (64 - (7 - (ESI % 8)) * 8)) & 0xFF); //Pulls and parses 8 bit data
            return;
        case BIT16_SIZE:
            DS = (uint16_t) ((stackMemory[ESI / 64] >> (64 - (3 - (ESI % 4)) * 16)) & 0xFFFF); //Pulls and parses 16 bit data
            return;
        default:
            return;
    }
}

//To write to memory
void VM::writeMem(uint8_t &data, uint64_t &temp) {
    switch (data) {
        case BIT8_SIZE:
            temp = 0xFF00000000000000; //Start bitmask

            //Move FF to the correct position
            for (int j = 0; j < EDI % 8; ++j)
                temp = temp >> 8;

            //Sets the memory area we are writing to to zero
            stackMemory[EDI / 64] = stackMemory[EDI / 64] & ~temp;

            //Moves data to start location
            temp = DS << 56;

            //Moves data to correct location
            for (int j = 0; j < EDI % 8; ++j)
                temp = temp >> 8;

            //Writes to location
            stackMemory[EDI / 64] = stackMemory[EDI / 64] | temp;
            return;
        case BIT16_SIZE:
            temp = 0xFFFF000000000000; //Start bitmask

            //Move FFFF into correct position
            for (int i = 0; i < EDI % 4; ++i)
                temp = temp >> 16;

            //Sets the memory area we are writing to to zero
            stackMemory[EDI / 64] = stackMemory[EDI / 64] & ~temp;

            //Move data to start location
            temp = DS << 48;

            //Moves data to correct location
            for (int j = 0; j < EDI % 4; ++j)
                temp = temp >> 16;

            //Writes to location
            stackMemory[EDI / 64] = stackMemory[EDI / 64] | temp;
            return;
        default:
            return;
    }
}

void VM::getMetaDataStack(uint8_t &data) {
    data = (uint8_t) ((stackMemory[ESI / 8] >> (64 - ((ESI % 8) + 1))) & 0xFF);
}

void VM::writeMetaDataStack(uint8_t &data, uint64_t &temp) {
    temp = 0xFF00000000000000; //Start bitmask

    //Move FF to the correct position
    for (int j = 0; j < EDI % 8; ++j)
        temp = temp >> 8;

    //Sets the memory area we are writing to to zero
    metadataStack[EDI / 64] = metadataStack[EDI / 64] & ~temp;

    //Moves data to start location
    temp = data << 56;

    //Moves data to correct location
    for (int j = 0; j < EDI / 8; ++j)
        temp = temp >> 8;

    //Writes to location
    metadataStack[EDI / 64] = metadataStack[EDI / 64] | temp;
}