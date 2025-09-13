#include <stdio.h>
#include <stdlib.h>

#include "VirtualMachine.h"

void readMemory(TVM *vm);
void writeMemory(TVM *vm);
int convertToPhysicalAddress(TVM *vm);