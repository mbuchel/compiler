//TODO: make a program later after the vm class is finished

//Currently using this file to test my code
#include <stdint-gcc.h>
#include "VM.h"

int main() {
    uint64_t code[2];
    code[0] = 0xFF;
    code[1] = 0xFD;

    VM temp;
    temp.load(code);
    temp.run();

    return 0;
}