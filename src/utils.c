#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "VM_memory.h"
#include "constants.h"

const char* MNEMONIC_NAMES[] = {
    "SYS", "JMP", "JZ", "JP", "JN", "JNZ", "JNP", "JNN", "NOT",
    "Invalid Operation Code", "Invalid Operation Code", "PUSH", "POP",
    "CALL", "RET",
    "STOP", "MOV", "ADD", "SUB", "MUL", "DIV", "CMP", "SHL", "SHR", "SAR",
    "AND", "OR", "XOR", "SWAP", "LDL", "LDH", "RND"};

const char* REGISTER_NAMES[] = {
    "LAR", "MAR", "MBR", "IP", "OPC", "OP1", "OP2",
    "SP", "BP", "RESERVADO", "EAX",
    "EBX", "ECX", "EDX", "EEX", "EFX", "AC", "CC",
    "RESERVADO", "RESERVADO", "RESERVADO", "RESERVADO",
    "RESERVADO", "RESERVADO", "RESERVADO", "RESERVADO",
    "CS", "DS", "ES", "SS", "KS",
    "PS"};

void setCC(TVM* vm, int value) {
    if (value < 0)  // N=1
        vm->reg[CC] |= 1 << 31;
    else  // N=0
        vm->reg[CC] &= 0x7FFFFFFF;
    if (value == 0)  // Z=1
        vm->reg[CC] |= 1 << 30;
    else  // Z=0
        vm->reg[CC] &= 0xBFFFFFFF;
}

int signExtend(unsigned int value, int nbytes) {
    int shift = (4 - nbytes) * 8;           // cuántos bits correr
    return (int)(value << shift) >> shift;  // extiende signo al castear
}

int getOp(TVM* vm, int registerValue) {
    int type = (registerValue & 0xFF000000) >> 24;  // obtengo el tipo de operando
    int opAux = registerValue & 0x00FFFFFF;         // obtengo el operando sin el tipo

    if (type == 0b01) {  // registro
        unsigned int sectRegister = (opAux & 0x000000C0) >> 6;
        unsigned int registro = opAux & 0x0000001F;
        if (sectRegister == 0)
            return vm->reg[registro];
        else if (sectRegister == 1)
            return vm->reg[registro] & 0x000000FF;
        else if (sectRegister == 2)
            return vm->reg[registro] & 0x0000FF00;
        else
            return vm->reg[registro] & 0x0000FFFF;
    } else if (type == 0b10) {     // inmediato
        return (opAux << 8) >> 8;  // extiendo el signo
    } else if (type == 0b11) {
        int registro = (opAux & 0x1F0000) >> 16;       // obtengo el registro
        int offset = (int16_t)((opAux & 0x0000FFFF));  // en [EDX + 4] el offset es 4 y extiendo el signo
        unsigned int cellSize = (opAux & 0xC00000) >> 22;
        unsigned int oldSegment = vm->reg[registro] >> 16;
        vm->reg[LAR] = vm->reg[registro] + offset;  // cargo LAR con el segmento y offset del operando
        if ((vm->reg[LAR] >> 16) != oldSegment) {
            printf("Error: Segmentation fault\n");
            exit(1);
        }

        if (cellSize == 0)
            vm->reg[MAR] = 0x00040000;
        else if (cellSize == 2)
            vm->reg[MAR] = 0x00020000;
        else
            vm->reg[MAR] = 0x00010000;
        // seteamos MAR para leer 4 bytes
        readMemory(vm);
        return vm->reg[MBR];
    }
    return 0;
}
void setOp(TVM* vm, int registerValue, int value) {
    int mask = 0x00FFFFFF;
    int type = (registerValue & 0xFF000000) >> 24;  // obtengo el tipo de operando
    int opAux = registerValue & mask;               // obtengo el operando sin el tipo
    if (type == 0b01) {                             // registro
        unsigned int sectRegister = (opAux & 0x000000C0) >> 6;
        unsigned int registro = opAux & 0x0000001F;
        if (sectRegister == 0)
            vm->reg[registro] = value;
        else if (sectRegister == 1)
            vm->reg[registro] = (vm->reg[registro] & 0xFFFFFF00) | (value & 0x000000FF);
        else if (sectRegister == 2)
            vm->reg[registro] = (vm->reg[registro] & 0xFFFF00FF) | (value & 0x0000FF00);
        else
            vm->reg[registro] = (vm->reg[registro] & 0xFFFF0000) | (value & 0x0000FFFF);
    } else if (type == 0b10) {  // inmediato
        exit(1);
    } else if (type == 0b11) {                             // memoria
        unsigned int registro = (opAux & 0x1F0000) >> 16;  // obtengo el registro
        unsigned int cellSize = (opAux & 0xC00000) >> 22;
        int offset = (int16_t)(opAux & 0x0000FFFF);  // obtengo el offset
        unsigned int oldSegment = vm->reg[registro] >> 16;
        vm->reg[LAR] = vm->reg[registro] + offset;  // cargo LAR con el segmento y offset del operando
        if (vm->reg[LAR] >> 16 != oldSegment) {     // verifico si sumando me pase del segmento
            printf("Error: Segmentation fault\n");
            exit(1);
        }
        vm->reg[MBR] = value;
        if (cellSize == 0)
            vm->reg[MAR] = 0x00040000;
        else if (cellSize == 2)
            vm->reg[MAR] = 0x00020000;
        else
            vm->reg[MAR] = 0x00010000;
        writeMemory(vm);
    }
}

void invalidOpCode(TVM* vm, int tipoOp1, int tipoOp2) {
    printf("Error: Invalid Mnemonic Code");
    exit(1);
}
void showRegisters(TVM* vm) {
    printf("\n----- Registros -----\n");
    for (int i = 0; i < 32; i++) {
        printf("%s: %08X\n", REGISTER_NAMES[i], vm->reg[i]);
    }
}

void showCodeSegment(TVM* vm) {
    printf("\n----- Segmento de Código -----\n");
    unsigned int base = vm->tableSeg[(vm->reg[CS] >> 16)].base;
    unsigned int size = vm->tableSeg[(vm->reg[CS] >> 16)].size;
    printf("Tamaño del segmento de código: %u bytes\n", size);
    for (int i = 0; i < size; i++) {
        printf("[%04x]: %X\n", base + i, vm->mem[base + i]);
    }
}
void showParamSegment(TVM* vm) {
    if (vm->reg[PS] == -1) {
        printf("No hay segmento de parámetros.\n");
        return;
    }
    printf("\n----- Segmento de Parámetros -----\n");
    unsigned int base = vm->tableSeg[0].base;
    unsigned int size = vm->tableSeg[0].size;

    printf("Tamaño del segmento de parámetros: %u bytes\n", size);
    printf("Base del segmento de parámetros: %u\n", base);

    for (unsigned int i = 0; i < size; i++) {
        // Mostrar string
        printf("Parametro en [%04x]: %02X\n", base + i, vm->mem[base + i]);
    }
}
void showStackSegment(TVM* vm) {
    if (vm->reg[SS] == -1) {
        printf("No hay segmento de stack.\n");
        return;
    }
    printf("\n----- Segmento de Stack -----\n");
    unsigned int base = vm->tableSeg[vm->reg[SS] >> 16].base;
    unsigned int size = vm->tableSeg[vm->reg[SS] >> 16].size;

    for (unsigned int i = 0; i < size; i++) {
        printf("[%04x]: %02X\n", base + i, vm->mem[base + i]);
    }
}
void showTSR(TVM* vm) {
    printf("\n----- Tabla de Segmentos -----\n");
    printf("Segmento |   Base   |   Size   \n");
    printf("-------------------------------\n");
    for (int i = 0; i < 8; i++) {
        printf("   %d    | %06X | %06X \n", i, vm->tableSeg[i].base, vm->tableSeg[i].size);
    }
}