#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

#include "VM_memory.h"
#include "constants.h"

void setCC(TVM *vm, int value) {
    if (value < 0)  // N=1
        vm->reg[CC] |= 1 << 31;
    else  // N=0
        vm->reg[CC] &= 0x7FFFFFFF;
    if (value == 0)  // Z=1
        vm->reg[CC] |= 1 << 30;
    else  // Z=0
        vm->reg[CC] &= 0xBFFFFFFF;
    printf("CC: %o\n", vm->reg[CC]);
}

int signExtend(unsigned int value, int nbytes) {
    int shift = (4 - nbytes) * 8;           // cuántos bits correr
    return (int)(value << shift) >> shift;  // extiende signo al castear
}

int getOp(TVM *vm, int registerValue) {
    int type = (registerValue & 0xFF000000) >> 24;  // obtengo el tipo de operando
    int opAux = registerValue & 0x00FFFFFF;         // obtengo el operando sin el tipo

    if (type == 0b01) {  // registro
        return vm->reg[opAux];
    } else if (type == 0b10) {     // inmediato
        return (opAux << 8) >> 8;  // extiendo el signo
    } else if (type == 0b11) {
        int registro = (opAux & 0x1F0000) >> 16;  // obtengo el registro
        int offset = opAux & 0x0000FFFF;
        printf("Registro: %x Offset: %x\n", registro, offset);  // obtengo el offset
        vm->reg[LAR] = vm->reg[registro] + offset;              // cargo LAR con segmento de datos y offset del operando
        vm->reg[MAR] = 0x00040000;
        // seteamos MAR para leer 4 bytes
        readMemory(vm);
        printf("Valor leido: %d\n", vm->reg[MBR]);
        return vm->reg[MBR];
    }
    return 0;
}
void setOp(TVM *vm, int registerValue, int value) {
    int mask = 0x00FFFFFF;
    int type = (registerValue & 0xFF000000) >> 24;  // obtengo el tipo de operando
    int opAux = registerValue & mask;               // obtengo el operando sin el tipo
    if (type == 0b01) {                             // registro
        vm->reg[opAux] = value;
    } else if (type == 0b10) {  // inmediato
        printf("Error: No se puede escribir en un inmediato.\n");
        exit(1);
    } else if (type == 0b11) {                    // memoria
        int registro = (opAux & 0x1F0000) >> 16;  // obtengo el registro
        int offset = opAux & 0x0000FFFF;          // obtengo el offset
        // cargo LAR con el contenido del registro (debera ser un puntero) y offset del operando
        vm->reg[LAR] = vm->reg[registro] + offset;
        vm->reg[MBR] = value;
        vm->reg[MAR] = 0x00040000;
        writeMemory(vm);
    }
}