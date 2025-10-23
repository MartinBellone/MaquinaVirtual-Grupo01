#include "VirtualMachine.h"

void MOV(TVM* vm, int tipoOp1, int tipoOp2);
void SWAP(TVM* vm, int tipoOp1, int tipoOp2);
void LDL(TVM* vm, int tipoOp1, int tipoOp2);
void LDH(TVM* vm, int tipoOp1, int tipoOp2);
void PUSH(TVM* vm, int tipoOp1, int tipoOp2);
void POP(TVM* vm, int tipoOp1, int tipoOp2);
void CALL(TVM* vm, int tipoOp1, int tipoOp2);
void RET(TVM* vm, int tipoOp1, int tipoOp2);