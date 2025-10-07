#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "VirtualMachine.h"

int main(int argc, char *argv[]) {
    TVM vm;
    VMParams params;
    parseArgs(argc, argv, &params);

    vm.argc = params.argc;
    buildParamSegment(&vm, &params);

    // Load a program into memory
    readFile(&vm, params.vmxFile);

    // Mostrar el segmento de código cargado
    // showCodeSegment(&vm);

    if (params.disassembly)
        executeDisassembly(&vm);

    // Execute the program
    executeProgram(&vm);
    return 0;
}
