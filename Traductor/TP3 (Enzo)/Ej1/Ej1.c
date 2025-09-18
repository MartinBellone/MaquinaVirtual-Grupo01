#include <stdio.h>
#include <stdlib.h>

int main(){
    int num, cant = 0;
    float prom = 0;
    scanf("%d", &num);
    while(num >= 0){
        cant++;
        prom += num;
        scanf("%d", &num);
    }
    prom /= cant;
    printf("%4.2f", prom);
    return 0;
}