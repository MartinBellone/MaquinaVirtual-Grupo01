#include "VirtualMachine.h"

void SYS(TVM *vm, int tipoOp1, int tipoOp2);
void STOP(TVM *vm, int tipoOp1, int tipoOp2);
void invalidOpCode(TVM *vm, int tipoOp1, int tipoOp2);