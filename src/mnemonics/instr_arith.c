#include "instr_arith.h"

#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "utils.h"

void ADD(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2;
    value1 = getOp(vm, vm->reg[OP1]);
    value2 = getOp(vm, vm->reg[OP2]);
    value1 += value2;
    printf("Valor de la suma: %X\n", value1);
    setCC(vm, value1);
    setOp(vm, vm->reg[OP1], value1);
}

void SUB(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2;
    value1 = getOp(vm, vm->reg[OP1]);
    value2 = getOp(vm, vm->reg[OP2]);
    value1 -= value2;
    setCC(vm, value1);
    setOp(vm, vm->reg[OP1], value1);
}

void MUL(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2;
    value1 = getOp(vm, vm->reg[OP1]);
    value2 = getOp(vm, vm->reg[OP2]);
    printf("Valor 1: %d Valor 2: %d\n", value1, value2);
    value1 *= value2;
    printf("Valor de la multiplicacion: %d\n", value1);
    setCC(vm, value1);
    setOp(vm, vm->reg[OP1], value1);
}

void DIV(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2;
    value1 = getOp(vm, vm->reg[OP1]);
    value2 = getOp(vm, vm->reg[OP2]);
    if (value2 == 0) {
        printf("Error: Division por cero.\n");
        exit(1);
    }
    vm->reg[AC] = value1 % value2;
    value1 /= value2;  // hace division entera porque ambos son enteros
    setCC(vm, value1);
    setOp(vm, vm->reg[OP1], value1);
}

void RND(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2;
    value1 = getOp(vm, vm->reg[OP1]);
    value2 = getOp(vm, vm->reg[OP2]);
    value1 = rand() % (value2 + 1);  // deberia sumar fuera y restar dentro del parentesis el minimo del intervalo pero siempre es 0
    setOp(vm, vm->reg[OP1], value1);
}
void SHL(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2;
    value1 = getOp(vm, vm->reg[OP1]);
    value2 = getOp(vm, vm->reg[OP2]);
    value1 = value1 << value2;
    setCC(vm, value1);
    setOp(vm, vm->reg[OP1], value1);
}

void SHR(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2, mascara = 0xFFFFFFFF;
    value1 = getOp(vm, vm->reg[OP1]);
    value2 = getOp(vm, vm->reg[OP2]);
    value1 &= ~(mascara << (32 - value2));
    /*

        Ej:

        value1 = 1111;
        value2 = 1;

        value1 >> value2
        En C quedaria 1111 (Mantiene signo) tendria que ser 0111

        Entonces nuestra mascara que es 1111 hago
        1111 << (4 - 1) = 1000
        ~1000 = 0111

        Value1 quedaria:
        1111 & 0111 = 0111

    */
    setCC(vm, value1);
    setOp(vm, vm->reg[OP1], value1);
}

void SAR(TVM *vm, int tipoOp1, int tipoOp2) {
    int value1, value2;
    value1 = getOp(vm, vm->reg[OP1]);
    value2 = getOp(vm, vm->reg[OP2]);
    value1 = value1 >> value2;
    setCC(vm, value1);
    setOp(vm, vm->reg[OP1], value1);
}