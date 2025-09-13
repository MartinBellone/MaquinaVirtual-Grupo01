#include "instr_sys.h"

#include <stdio.h>
#include <stdlib.h>

#include "VM_memory.h"
#include "constants.h"

void SYS(TVM *vm, int tipoOp1, int tipoOp2) {
    int call = vm->reg[OP1];
    call = call & 0x000000FF;  // Aislo los 8 bits menos significativos
    int tamanioCelda = (vm->reg[ECX] & 0xFFFF0000) >> 16;
    int cantLecturas = vm->reg[ECX] & 0x0000FFFF;
    printf("Call: %X Tamanio celda: %X Cantidad lecturas: %X\n", call, tamanioCelda, cantLecturas);
    printf("EAX: %X \n", vm->reg[EAX]);
    if (vm->reg[EAX] < 0 || vm->reg[EAX] > 10)
        exit(1);

    vm->reg[LAR] = vm->reg[EDX];        // Cargo LAR con la direcicon logica
    vm->reg[MAR] = tamanioCelda << 16;  // Cargo MAR con la cantidad de bytes a leer
    printf("LAR: %X MAR: %X\n", vm->reg[LAR], vm->reg[MAR]);
    for (int i = 0; i < cantLecturas; i++)
        if (call == 1) {
            printf("[%08x]: ", vm->reg[EDX]);
            if (vm->reg[EAX] == 0) {
                char valor;
                scanf(" %c", &valor);
                vm->reg[MBR] = valor;
                writeMemory(vm);
            } else if (vm->reg[EAX] == 1) {
                int valor;
                scanf(" %d", &valor);
                vm->reg[MBR] = valor;
                writeMemory(vm);
            } else if (vm->reg[EAX] == 2 | vm->reg[EAX] == 4 | vm->reg[EAX] == 8) {
                unsigned short int valor;
                scanf("%hu", &valor);
                vm->reg[MBR] = valor;
                writeMemory(vm);
            } else
                // No es una base valida
                exit(1);
        } else if (call == 2)
            if (vm->reg[EAX] == 0) {
                int valor;
                readMemory(vm);
                valor = vm->reg[MBR];
                printf("%d", valor);
            } else if (vm->reg[EAX] == 1) {
                int valor;
                readMemory(vm);
                valor = vm->reg[MBR];
                printf("%d\n", valor);
            } else if (vm->reg[EAX] == 2 | vm->reg[EAX] == 4 | vm->reg[EAX] == 8) {
                unsigned short int valor;
                readMemory(vm);
                valor = vm->reg[MBR];
                printf("%hu", valor);
            } else
                // No es una base valida
                exit(1);
}

void STOP(TVM *vm, int tipoOp1, int tipoOp2) {
    exit(0);
}
void invalidOpCode(TVM *vm, int tipoOp1, int tipoOp2) {
    printf("Error: Invalid Mnemonic Code");
    exit(1);
}