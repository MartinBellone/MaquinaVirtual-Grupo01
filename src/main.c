#include <stdio.h>
#include <stdlib.h>

#include "VirtualMachine.h"
#define LAR 0
#define MAR 1
#define MBR 2
int main(int argc, char *argv[]) {
    TVM vm;
    int dFlag;
    // if (vm == NULL) {
    //     fprintf(stderr, "Error allocating memory for virtual machine.\n");
    //     return 1;
    // }


    if(argc == 1){
        printf("No hay ningun archivo para ejecutar.\n");
        return 1;
    }


    //  Parameters check
    if (argc > 2)
        dFlag = (strcmp(argv[2], "-d") == 0) ? 1 : 0;
        
    // Load a program into memory
    readFile(&vm, argv[1]);

    // Mostrar el segmento de código cargado
    // showCodeSegment(&vm);

    if(dFlag)
        executeDisassembly(&vm);

    // Execute the program
    executeProgram(&vm);
    return 0;
}
