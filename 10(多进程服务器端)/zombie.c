
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(){
    pid_t pid = fork();
    if( pid == 0 ){
        puts("Hi, I am child Process." );
    }else{
        printf("Child Process ID: %d \n",pid );
        sleep( 30 );
    }
    if( pid == 0)
        printf("End child process\n" );
    else
        printf("End parent process\n" );
    return 0;
}
