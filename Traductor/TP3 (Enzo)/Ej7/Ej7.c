#include <stdio.h>
#include <stdlib.h>

int main(){
    int num, contador = 0, primo = 0;
    scanf("%d", &num);
    while(contador <= num && !primo){
        if(num % contador == 0)
            primo = 1;
        contador++;
    }
    return 0;
}