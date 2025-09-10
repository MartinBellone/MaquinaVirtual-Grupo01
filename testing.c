#include <stdio.h>
#include <stdlib.h>

#include "VirtualMachine.h"
#define LAR 0
#define MAR 1
#define MBR 2
int main() {
    TVM vm;
    // if (vm == NULL) {
    //     fprintf(stderr, "Error allocating memory for virtual machine.\n");
    //     return 1;
    // }

    // Load a program into memory
    readFile(&vm, "sample.vmx");
    // // tiene que venir el MAR seteado con la cantidad de bytes a escribir y el MBR con los datos a escribir
    // vm.reg[MAR] = 0x00040000;
    // vm.reg[MBR] = 0xFFFFFFF1;
    // vm.reg[LAR] = 0x00010006;
    // writeMemory(&vm);
    // printf("Value to write: 0x%X\n", vm.reg[MBR]);
    // vm.reg[MAR] = 0x00040000;
    // vm.reg[MBR] = 0x0000000F;
    // vm.reg[LAR] = 0x00010006;
    // readMemory(&vm);
    // printf("Memory: 0x%X\n", vm.reg[MBR]);
    // printf("Memory: %d\n", vm.reg[MBR]);

    vm.reg[MAR] = 0x00020000;
    vm.reg[MBR] = 300;
    vm.reg[LAR] = 0x00010006;
    writeMemory(&vm);
    vm.reg[MAR] = 0x00020000;
    vm.reg[LAR] = 0x00010006;
    readMemory(&vm);
    printf("2 bytes, valor escrito=300, leido=%d (0x%X)\n", vm.reg[MBR], vm.reg[MBR]);
    vm.reg[MAR] = 0x00020000;
    vm.reg[MBR] = -300;
    vm.reg[LAR] = 0x00010006;
    writeMemory(&vm);
    vm.reg[MAR] = 0x00020000;
    vm.reg[LAR] = 0x00010006;
    readMemory(&vm);
    printf("2 bytes, valor escrito=-300, leido=%d (0x%X)\n", vm.reg[MBR], vm.reg[MBR]);
    // Clean up

    return 0;
}
