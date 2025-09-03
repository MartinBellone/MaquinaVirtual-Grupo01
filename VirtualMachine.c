#include "VirtualMachine.h"
#include <stdio.h>
#include <stdlib.h>

#define LAR 0 
#define MAR 1
#define MBR 2
#define IP 3 //registro del contador de instrucciones
#define OPC 4
#define OP1 5
#define OP2 6
#define EAX 10
#define EBX 11
#define ECX 12
#define EDX 13
#define EEX 14
#define EFP 15
#define AC 16  
#define CC 17              
#define CS 26
#define DS 27

void readMemory(){

}

void writeMemory(){

}

void readFile(TVM *vm, char name[]){     //funcion para leer el vmx
    FILE *arch;
    int i=0; //direccion de memoria a guardar el byte
    char c;

    arch=fopen(name,"rb");
    if (arch==NULL)
        printf("ERROR al abrir el archivo");
    else
        while (fread(&c,sizeof(char),1,arch)==1){   //TODO: revisar fread
            vm->mem[i]=c;
            i++;
        }
        initVm(vm,i); 
}

void readInstruction(TVM *vm){
    char instruction;
    int maskOPC=0b00011111;  //mascara para obtener el codigo de operacion
    int maskTOP1=0b00110000;  //mascara para obtener el primer operando
    int maskTOP2=0b11000000;  //mascara para obtener el segundo operando
    int TOP1,TOP2;
    instruction= vm->mem[vm->reg[IP]];

    TOP2= (instruction & maskTOP2)>>6;
    TOP1= (instruction & maskTOP1)>>4;
    vm->reg[OPC]= instruction & maskOPC;
}
