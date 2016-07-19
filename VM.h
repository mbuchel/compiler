//
// Created by Micheal Buchel on 18/07/16.
//
// Decided to use a simple VM as opposed to x86 ASM (Ironically enough using modified Intel x86 syntax asm)
// Using the sites (http://www.sandpile.org/x86/opc_1.htm) and (http://www.cs.virginia.edu/~evans/cs216/guides/x86.html)

#ifndef COMPILER_VM_H
#define COMPILER_VM_H

//VM specs
#define PROGRAM_SPACE 1000
#define STACK_SPACE 1000
#define HEAP_SPACE 1000

//OP-code mask
#define OPCODE_BITMASK 0xFF00000000000000

//Hex OPCODES
//Movement instructions
#define MOV 0x88 //Moves data from one register to another
#define PUSH 0xFF //Pushes operand to stack memory and increments the ESP
#define POP 0x5F //Removes from stack and decreses ESP
#define LEA 0x8D //Load effective address (pointers)
//Arithmetic and logic instructions
#define ADD 0x05 //Adds 2 operands
#define SUB 0x2A //Subtracts 2 operands second form first
#define INC 0x47 //Increments the bit value by one NOTE: potential sign issues from here
#define DEC 0x4F //Decrements the bit value by one NOTE: potential sign issues from here
#define IMUL 0x69 //Integer multiplication
#define IDIV 0x6B //Could not find the op-code for this so I used the second IMUL
#define AND 0x25 //Bitwise and operator
#define OR 0x0D //Bitwise or operator
#define XOR 0x35 //Bitwise xor operator
#define NOT 0x3D //Using the CMP op-code for not
#define NEG 0x3A //2's complement (using another CMP op-code)
#define SHL 0x18 //Shift to the left (using SBB op-code)
#define SHR 0x19 //Shift to the right (using SBB op-code)
//Control flow Instructions
#define JMP 0xE9 //Jumps to another area in the code
#define JE 0x70 //Jumps to another area in the code if the 2 operands for CMP are equal (using JO)
#define JNE 0x71 //Jumps to another area in the code if the 2 operands for CMP are not equal (using JNO)
#define JZ 0x74 //Jumps if last register used is equal to zero
#define JNZ 0x75 //Jumps if the last register used is not equal to zero
#define JG 0x7D //Jumps if greater then is the value of the last CMP
#define JGE 0x7F //Jumps if greater then or equal is the value of the last CMP
#define JL 0x7C //Jumps if less then is the value of the last CMP
#define JLE 0x7E //Jumps if less then or equal is the value of the last CMP
#define CMP 0x38 //Compares 2 bit fields and sees if they are equal
#define CALL 0x9A //Subroutine call
#define RET 0xC3 //Subroutine return
#define EXIT_CODE 0xFD //Using STD op-code for this

#include <cstdint>

class VM {
private:
    uint8_t ESP; //Grows up (as opposed to x86 which grows down)
    uint64_t codeMemory[PROGRAM_SPACE]; //Where the program is stored currently it has 64 kilobits
    uint64_t stackMemory[STACK_SPACE]; //Stack address which has 64 kilobits
    uint64_t heapMemory[HEAP_SPACE]; //Where temp variables and virtual data goes
    uint64_t metadataStack[STACK_SPACE]; //Defines data types for stack
    uint64_t metadataHeap[HEAP_SPACE]; //Defines the data types for heap
public:
    void load(uint64_t code[]); //Loads program into the code
    void run(); //Runs the program
};


#endif //COMPILER_VM_H