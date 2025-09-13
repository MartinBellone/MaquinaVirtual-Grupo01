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
    int aux = getOp(vm, OP2);
    setOp(vm, OP2, OP1);
    setOp(vm, OP1, aux);
}

void LDL(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2, aux, mask = 0xFFFF0000;
    value1 = getOp(vm, vm->reg[OP1]);
    value2 = getOp(vm, vm->reg[OP2]);
    value1 &= mask;
    value1 |= value2;
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
