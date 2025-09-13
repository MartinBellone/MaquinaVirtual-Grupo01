#include <stdio.h>
#include <stdlib.h>

#include "VirtualMachine.h"
#define LAR 0
#define MAR 1
#define MBR 2
int main() {
    TVM vm;
    // if (vm == NULL) {
    //     fprintf(stderr, "Error allocating memory for virtual machine.\n");
    //     return 1;
    // }

    // Load a program into memory
    readFile(&vm, "./testFiles/Ej1.vmx");

    // Mostrar el segmento de código cargado
    // showCodeSegment(&vm);

    // Execute the program
    executeProgram(&vm);

    // Clean up

    return 0;
}
