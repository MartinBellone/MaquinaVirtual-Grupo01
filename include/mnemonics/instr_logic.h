#include "VirtualMachine.h"

void AND(TVM *vm, int tipoOp1, int tipoOp2);
void OR(TVM *vm, int tipoOp1, int tipoOp2);
void XOR(TVM *vm, int tipoOp1, int tipoOp2);
void NOT(TVM *vm, int tipoOp1, int tipoOp2);
void CMP(TVM *vm, int tipoOp1, int tipoOp2);

void JMP(TVM *vm, int tipoOp1, int tipoOp2);
void JZ(TVM *vm, int tipoOp1, int tipoOp2);
void JP(TVM *vm, int tipoOp1, int tipoOp2);
void JN(TVM *vm, int tipoOp1, int tipoOp2);
void JNZ(TVM *vm, int tipoOp1, int tipoOp2);
void JNP(TVM *vm, int tipoOp1, int tipoOp2);
void JNN(TVM *vm, int tipoOp1, int tipoOp2);