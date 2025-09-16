#include <stdio.h>
#include <stdlib.h>

#include "VirtualMachine.h"
#define LAR 0
#define MAR 1
#define MBR 2
int main(int argc, char *argv[]) {
    TVM vm;
    int dFlag = 0;
    // if (vm == NULL) {
    //     fprintf(stderr, "Error allocating memory for virtual machine.\n");
    //     return 1;
    // }

    //  Parameters check
    if (argc >= 2)
        dFlag = (strcmp(argv[2], "-d") == 0) ? 1 : 0;

    // Load a program into memory
    readFile(&vm, argv[1], dFlag);

    // Mostrar el segmento de código cargado
    // showCodeSegment(&vm);

    // Execute the program
    executeProgram(&vm);

    // Clean up

    return 0;
}
