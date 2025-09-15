#include "VirtualMachine.h"
#include "constants.h"
void setCC(TVM *vm, int value);
int signExtend(unsigned int value, int nbytes);

void setOp(TVM *vm, int registerValue, int value);
int getOp(TVM *vm, int registerValue);

extern const char *MNEMONIC_NAMES[];