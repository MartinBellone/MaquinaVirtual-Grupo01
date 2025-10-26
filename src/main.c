#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "VirtualMachine.h"

int main(int argc, char* argv[]) {
    TVM vm;
    VMParams params;
    parseArgs(argc, argv, &params, &vm);

    vm.argc = params.argc;
    buildParamSegment(&vm, &params);

    // Load a program into memory
    if (params.vmxFile)  // Si hay .vmx lo leo
        readFileVMX(&vm, params.vmxFile);
    else if (params.vmiFile)  // Si no habia .vmx pero si .vmi lo leo para ejecutar la imagen
        readFileVMI(&vm, params.vmiFile);
    else {
        printf("No se especificó archivo .vmx o .vmi\n");
        exit(1);
    }

    if (params.disassembly)
        executeDisassembly(&vm);

    // Execute the program
    executeProgram(&vm);
    return 0;
}
