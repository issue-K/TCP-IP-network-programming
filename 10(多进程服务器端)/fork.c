
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int gval = 10;

int main(){
    pid_t pid;
    int lval = 20;
    gval++, lval += 5;
    pid = fork();
    if( pid ){
        gval -=  2, lval -= 2;
    }else{
        gval += 2, lval += 2;
    }
    if( pid==0 ){
        printf("Child proc:[%d %d]",gval,lval );
    }else{
        printf("Parent proc: [%d %d] \n",gval,lval );
    }
    return 0;
}
