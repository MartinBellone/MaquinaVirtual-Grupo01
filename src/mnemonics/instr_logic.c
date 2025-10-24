#include "instr_logic.h"

#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "utils.h"

void JMP(TVM *vm, int tipoOp1, int tipoOp2) {
    int direccion = getOp(vm, vm->reg[OP1]);
    vm->reg[IP] &= 0xFFFF0000;
    vm->reg[IP] |= direccion;
}

void JZ(TVM *vm, int tipoOp1, int tipoOp2) {
    int direccion = getOp(vm, vm->reg[OP1]);
    unsigned int Z = (vm->reg[CC] & 0x40000000) >> 30;  // Aislo el bit Z
    unsigned int N = (vm->reg[CC] & 0x80000000) >> 31;  // Aislo el bit N
    if (Z == 1 && N == 0) {
        vm->reg[IP] &= 0xFFFF0000;
        vm->reg[IP] |= direccion;
    }
}

void JP(TVM *vm, int tipoOp1, int tipoOp2) {
    int direccion = getOp(vm, vm->reg[OP1]);
    unsigned int Z = (vm->reg[CC] & 0x40000000) >> 30;  // Aislo el bit Z
    unsigned int N = (vm->reg[CC] & 0x80000000) >> 31;  // Aislo el bit N
    if (N == 0 && Z == 0) {
        vm->reg[IP] &= 0xFFFF0000;
        vm->reg[IP] |= direccion;
    }
}

void JN(TVM *vm, int tipoOp1, int tipoOp2) {
    int direccion = getOp(vm, vm->reg[OP1]);
    unsigned int Z = (vm->reg[CC] & 0x40000000) >> 30;  // Aislo el bit Z
    unsigned int N = (vm->reg[CC] & 0x80000000) >> 31;  // Aislo el bit N
    if (N == 1 && Z == 0) {
        vm->reg[IP] &= 0xFFFF0000;
        vm->reg[IP] |= direccion;
    }
}

void JNZ(TVM *vm, int tipoOp1, int tipoOp2) {
    int direccion = getOp(vm, vm->reg[OP1]);
    unsigned int Z = (vm->reg[CC] & 0x40000000) >> 30;  // Aislo el bit Z
    // tengo en cuenta solo Z porque si Z=0, N puede ser 0 o 1
    if (Z == 0) {
        vm->reg[IP] &= 0xFFFF0000;
        vm->reg[IP] |= direccion;
    }
}

void JNP(TVM *vm, int tipoOp1, int tipoOp2) {
    int direccion = getOp(vm, vm->reg[OP1]);
    unsigned int Z = (vm->reg[CC] & 0x40000000) >> 30;  // Aislo el bit Z
    unsigned int N = (vm->reg[CC] & 0x80000000) >> 31;  // Aislo el bit N
    if (N == 1 && Z == 0 || N == 0 && Z == 1) {         // si el resultado es negativo o cero
        vm->reg[IP] &= 0xFFFF0000;
        vm->reg[IP] |= direccion;
    }
}

void JNN(TVM *vm, int tipoOp1, int tipoOp2) {
    int direccion = getOp(vm, vm->reg[OP1]);
    unsigned int N = (vm->reg[CC] & 0x80000000) >> 31;  // Aislo el bit N
    // tengo en cuenta solo N porque si N=0, Z puede ser 0 o 1
    if (N == 0) {
        vm->reg[IP] &= 0xFFFF0000;
        vm->reg[IP] |= direccion;
    }
}
void AND(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2;
    value1 = getOp(vm, vm->reg[OP1]);
    value2 = getOp(vm, vm->reg[OP2]);
    value1 = value1 & value2;
    setCC(vm, value1);
    setOp(vm, vm->reg[OP1], value1);
}

void OR(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2;
    value1 = getOp(vm, vm->reg[OP1]);
    value2 = getOp(vm, vm->reg[OP2]);
    value1 = value1 | value2;
    setCC(vm, value1);
    setOp(vm, vm->reg[OP1], value1);
}

void XOR(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2;
    value1 = getOp(vm, vm->reg[OP1]);
    value2 = getOp(vm, vm->reg[OP2]);
    value1 = value1 ^ value2;
    setCC(vm, value1);
    setOp(vm, vm->reg[OP1], value1);
}

void NOT(TVM *vm, int tipoOp1, int tipoOp2) {
    int value = getOp(vm, vm->reg[OP1]);
    value = ~value;
    setOp(vm, vm->reg[OP1], value);
    setCC(vm, value);
}

void CMP(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2, result;
    // Falla aca porque se esta guardando mal el operando de memoria
    value1 = getOp(vm, vm->reg[OP1]);
    value2 = getOp(vm, vm->reg[OP2]);
    result = value1 - value2;
    setCC(vm, result);
}