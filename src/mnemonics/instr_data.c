#include "instr_data.h"

#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "utils.h"

void MOV(TVM *vm, int tipoOp1, int tipoOp2) {
    int value2;
    value2 = getOp(vm, vm->reg[OP2]);
    setOp(vm, vm->reg[OP1], value2);
}
void SWAP(TVM *vm, int tipoOp1, int tipoOp2) {
    int aux = getOp(vm, vm->reg[OP2]);
    setOp(vm, vm->reg[OP2], getOp(vm, vm->reg[OP1]));
    setOp(vm, vm->reg[OP1], aux);

    // printf("SWAP: OP1= %d, OP2= %d\n", getOp(vm, OP1), getOp(vm, OP2));
}

void LDL(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2, aux, mask = 0xFFFF0000;
    value1 = getOp(vm, vm->reg[OP1]);
    value2 = getOp(vm, vm->reg[OP2]);
    value1 &= mask;
    value1 |= value2 & 0x0000FFFF;
    // value1 |= value2;
    setOp(vm, vm->reg[OP1], value1);
}

void LDH(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2, aux, mask = 0x0000FFFF;
    value1 = getOp(vm, vm->reg[OP1]);
    value2 = getOp(vm, vm->reg[OP2]);
    value1 &= mask;
    value1 |= value2 << 16;
    setOp(vm, vm->reg[OP1], value1);
}

void PUSH(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1;

    vm->reg[SP] -= 4;
    if (vm->reg[SP] < vm->reg[SS]) {
        printf("Error: Stack overflow.\n");
        exit(1);
    }
    value1 = getOp(vm, vm->reg[OP1]);
    // Extiendo el signo
    signExtend(value1, 4);
    // Almaceno el valor en la pila (memoria)
    vm->mem[vm->reg[SP]] = (value1 >> 24) & 0xFF;       // byte más significativo
    vm->mem[vm->reg[SP] + 1] = (value1 >> 16) & 0xFF;   // segundo byte
    vm->mem[vm->reg[SP] + 2] = (value1 >> 8) & 0xFF;    // tercer byte
    vm->mem[vm->reg[SP] + 3] = value1 & 0xFF;           // byte menos significativo
}

void POP(TVM *vm, int tipoOp1, int tipoOp2) {
    if (vm->reg[SP] && 0x0000FFFF >= vm->tableSeg[vm->reg[SS] >> 16].size - 3) {
        printf("Error: Stack underflow.\n");
        exit(1);
    }
    int value1;
    // Recupero el valor de la pila (memoria)
    value1 = (vm->mem[vm->reg[SP]] << 24) | (vm->mem[vm->reg[SP] + 1] << 16) | (vm->mem[vm->reg[SP] + 2] << 8) | vm->mem[vm->reg[SP] + 3];
    signExtend(value1, 4);
    // Almaceno el valor en el operando
    setOp(vm, vm->reg[OP1], value1);
    vm->reg[SP] += 4;
}

void CALL(TVM *vm, int tipoOp1, int tipoOp2) {
    // Guardo la direccion de retorno en la pila
    vm->reg[SP] -= 4;
    if (vm->reg[SP] < vm->reg[SS]) {
        printf("Error: Stack overflow.\n");
        exit(1);
    }
    vm->mem[vm->reg[SP]] = (vm->reg[IP] >> 24) & 0xFF;       // byte más significativo
    vm->mem[vm->reg[SP] + 1] = (vm->reg[IP] >> 16) & 0xFF;   // segundo byte
    vm->mem[vm->reg[SP] + 2] = (vm->reg[IP] >> 8) & 0xFF;    // tercer byte
    vm->mem[vm->reg[SP] + 3] = vm->reg[IP] & 0xFF;           // byte menos significativo

    int direccion = getOp(vm, vm->reg[OP1]);
    // Salto a la direccion de la subrutina
    vm->reg[IP] &= 0xFFFF0000;
    vm->reg[IP] |= direccion;
}

void RET(TVM *vm, int tipoOp1, int tipoOp2) {
    if (vm->reg[SP] && 0x0000FFFF >= vm->tableSeg[vm->reg[SS] >> 16].size - 3) {
        printf("Error: Stack underflow.\n");
        exit(1);
    }
    // Recupero la direccion de retorno de la pila
    int direccion = (vm->mem[vm->reg[SP]] << 24) | (vm->mem[vm->reg[SP] + 1] << 16) | (vm->mem[vm->reg[SP] + 2] << 8) | vm->mem[vm->reg[SP] + 3];
    vm->reg[SP] += 4;
    // Salto a la direccion de retorno
    vm->reg[IP] = direccion;
}