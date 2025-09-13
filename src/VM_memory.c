#include "VM_memory.h"

#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "utils.h"

int convertToPhysicalAddress(TVM *vm) {
    int segment, baseSeg, offSeg;
    segment = (vm->reg[LAR] & 0xFFFF0000) >> 16;  // obtengo el segmento

    if (segment > 7) {  // si el segmento es mayor a 7, error
        printf("Error: Segmentation fault.\n");
        exit(1);
    }

    baseSeg = vm->tableSeg[segment].base;
    offSeg = vm->reg[LAR] & 0x0000FFFF;
    printf("Base: 0x%X Offset: 0x%X\n", baseSeg, offSeg);
    return 0x00000000 | (baseSeg + offSeg);
}
void readMemory(TVM *vm) {
    int physAddr = convertToPhysicalAddress(vm);
    // printf("Physical Address to read: 0x%X\n", physAddr);
    //  tiene que venir el MAR seteado con la cantidad de bytes a leer
    vm->reg[MAR] |= physAddr;
    int bytesToRead = (vm->reg[MAR] & 0xFFFF0000) >> 16;
    vm->reg[MBR] = 0x00000000;  // inicializo MBR en 0

    for (int i = 1; i <= bytesToRead; i++) {
        // printf("Reading from memory address 0x%X: 0x%X\n", vm->reg[MAR] + i - 1, vm->mem[physAddr + i - 1]);
        vm->reg[MBR] |= (vm->mem[physAddr + i - 1] << (8 * (bytesToRead - i)));  // leo byte a byte
    }

    // vm->reg[MBR] = vm->mem[physAddr];
    printf("Value read: 0x%X\n", vm->reg[MBR]);
    vm->reg[MBR] = signExtend(vm->reg[MBR], bytesToRead);
}

void writeMemory(TVM *vm) {
    int physAddr = convertToPhysicalAddress(vm);
    // printf("Writting...\n");
    //  tiene que venir el MAR seteado con la cantidad de bytes a escribir y el MBR con los datos a escribir
    vm->reg[MAR] |= physAddr;
    // printf("Physical Address to write: 0x%X\n", vm->reg[MAR]);
    int bytesToWrite = (vm->reg[MAR] & 0xFFFF0000) >> 16;
    int address = (vm->reg[MAR] & 0x0000FFFF);
    for (int i = 0; i < bytesToWrite; i++) {
        // 0x0004000A
        // printf("Writing to memory address 0x%X: 0x%X\n", vm->reg[MAR] + i, vm->reg[MBR] >> (8 * (bytesToWrite - i - 1)) & 0xFF);
        vm->mem[address + i] = vm->reg[MBR] >> (8 * (bytesToWrite - i - 1)) & 0xFF;  // escribo byte a byte
    }
}