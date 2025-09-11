#include "VirtualMachine.h"
#include <stdio.h>
#include <stdlib.h>


int main( ) {
    TVM vm;
    // if (vm == NULL) {
    //     fprintf(stderr, "Error allocating memory for virtual machine.\n");
    //     return 1;
    // }
      

    // Load a program into memory
    readFile(&vm, "sample.vmx");

    // Mostrar el segmento de código cargado
    //showCodeSegment(&vm);

    // Execute the program
    executeProgram(&vm);



    // Clean up
    
    return 0;
}

