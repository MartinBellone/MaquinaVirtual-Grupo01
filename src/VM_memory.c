#include "VM_memory.h"

#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "utils.h"

unsigned int convertToPhysicalAddress(TVM* vm) {
    int segment, baseSeg, offSeg;
    segment = (vm->reg[LAR] & 0xFFFF0000) >> 16;  // obtengo el segmento
    if (segment > 7) {                            // si el segmento es mayor a 7, error
        printf("Error: Segmentation fault.\n");
        exit(1);
    }

    baseSeg = vm->tableSeg[segment].base;
    offSeg = vm->reg[LAR] & 0x0000FFFF;
    int result = 0x00000000 | (baseSeg + offSeg);
    if (result > (baseSeg + vm->tableSeg[segment].size - 1)) {  // si la direccion fisica es mayor a la base + size del segmento, error
        printf("Error: Segmentation fault.\n");
        exit(1);
    }
    return result;
}
void readMemory(TVM* vm) {
    unsigned short int esParamSegment = 0;
    if (vm->reg[PS] != -1 && ((vm->reg[LAR] >> 16) == (vm->reg[PS] >> 16))) {
       // printf("Leyendo del segmento de parametros\n");
        esParamSegment = 1;
    }
    int physAddr = convertToPhysicalAddress(vm);
    //printf("Direccion fisica a leer: %04X\n", physAddr);
    //  tiene que venir el MAR seteado con la cantidad de bytes a leer
    vm->reg[MAR] |= physAddr;
    unsigned int bytesToRead = (vm->reg[MAR] & 0xFFFF0000) >> 16;

    vm->reg[MBR] = 0x00000000;  // inicializo MBR en 0
    //printf("Bytes to read: %d\n", bytesToRead);
    if (!esParamSegment) {
        for (unsigned int i = 1; i <= bytesToRead; i++) {
            vm->reg[MBR] |= (vm->mem[physAddr + i - 1] << (8 * (bytesToRead - i)));  // leo byte a byte
        }
        vm->reg[MBR] = signExtend(vm->reg[MBR], bytesToRead);
    } else {
        unsigned int i = physAddr;
        while (vm->mem[i] != '\0') {
            vm->reg[MBR] |= (vm->mem[i] << (8 * (i - physAddr)));  // leo byte a byte
            //printf("Leyendo byte %02X del segmento de parametros\n", vm->mem[i]);
            i++;
        }
    }
    //printf("Valor leído: %08X\n", vm->reg[MBR]);
}

void writeMemory(TVM* vm) {
    int physAddr = convertToPhysicalAddress(vm);
    //  tiene que venir el MAR seteado con la cantidad de bytes a escribir y el MBR con los datos a escribir
    vm->reg[MAR] |= physAddr;
    unsigned int bytesToWrite = (vm->reg[MAR] & 0xFFFF0000) >> 16;
    int address = (vm->reg[MAR] & 0x0000FFFF);
    for (unsigned int i = 0; i < bytesToWrite; i++) {
        vm->mem[address + i] = vm->reg[MBR] >> (8 * (bytesToWrite - i - 1)) & 0xFF;  // escribo byte a byte
    }
}