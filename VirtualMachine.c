#include "VirtualMachine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void initTSR(TVM * vm, char *size){
    vm->tableSeg[0].base=0;
    strcpy(vm->tableSeg[0].size,size);
    strcpy(vm->tableSeg[1].size,size);
    int cantBytes = atoi(size);
    vm->tableSeg[1].size = 16384 - cantBytes;
}

void readFile(TVM *vm, char name[]){     //funcion para leer el vmx
    FILE *arch;
    int i=0; //direccion de memoria a guardar el byte
    char c, size[2], header[8];

    arch=fopen(name,"rb");
    if (arch==NULL)
         printf("ERROR al abrir el archivo");
    else{
        // Lectura del identificador y version del archivo
        fread(header, 5*sizeof(char), 5, arch);
        if(strcmp(header, "VMX251") == 0){
            // Leo tamaño del archivo
            fread(size, 2*sizeof(char), 2, arch);

            // Inicializo tabla de segmentos
            initTSR(vm, size);

            if (vm->tableSeg[1].size <= 0){  //si se supera el tamaño del segmento de codigo, salir
                 printf("Error: El programa es demasiado grande para la memoria asignada.\n");
                }
            else{
                 // Leer codigo
                while (fread(&c,sizeof(char),1,arch)==1){ 
                    vm->mem[i]=c;
                    i++;
                }
                initVm(vm);
               
            }
        }
        else
          printf("Error: Formato de archivo incorrecto.\n");
        fclose(arch);
    } 
    //TODO cambiar los prints
        }
void initVm(TVM *vm){
    vm->reg[CS]=0;  //segmento de codigo
    vm->reg[IP]= vm->reg[CS];  //contador de instrucciones apunta al inicio del segmento de codigo
    vm->reg[DS]= 1 << 16;  //segmento de datos
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
    //TODO puntero a funcion segun OPC
}
