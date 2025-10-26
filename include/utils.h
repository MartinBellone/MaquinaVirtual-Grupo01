#include "VirtualMachine.h"
#include "constants.h"
void setCC(TVM* vm, int value);
int signExtend(unsigned int value, int nbytes);

void setOp(TVM* vm, int registerValue, int value);
int getOp(TVM* vm, int registerValue);

void showRegisters(TVM* vm);
void showCodeSegment(TVM* vm);
void showParamSegment(TVM* vm);
void showStackSegment(TVM* vm);
void invalidOpCode(TVM* vm, int tipoOp1, int tipoOp2);
extern const char* MNEMONIC_NAMES[];
extern const char* REGISTER_NAMES[];