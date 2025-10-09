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
    if(params.vmxFile){ // Si hay .vmx lo leo
        readFileVMX(&vm, params.vmxFile);
        if(params.vmiFile) // Si tambien habia .vmi genero la imagen
            writeFileVMI(&vm, params.vmiFile);
    }
    else
        if(params.vmiFile) // Si no habia .vmx pero si .vmi lo leo para ejecutar la imagen
            readFileVMI(&vm, params.vmiFile);
        else
            print("No se especificó archivo .vmx o .vmi\n");
            exit(1);

    // Mostrar el segmento de código cargado
    // showCodeSegment(&vm);

    if (params.disassembly)
        executeDisassembly(&vm);

    // Execute the program
    executeProgram(&vm);
    return 0;
}
