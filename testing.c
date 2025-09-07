#include "VirtualMachine.h"
#include <stdio.h>
#include <stdlib.h>


int main( ) {
    TVM vm;
    // if (vm == NULL) {
    //     fprintf(stderr, "Error allocating memory for virtual machine.\n");
    //     return 1;
    // }

    // Initialize the virtual machine
    initVm(&vm);

    // Load a program into memory
    readFile(&vm, "sample.vmx");



    // Clean up
    
    return 0;
}

