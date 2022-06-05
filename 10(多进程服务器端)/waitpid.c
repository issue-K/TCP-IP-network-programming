
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(){
    int status;
    pid_t pid = fork();
    if( pid == 0 ){
        sleep( 30 );
        return 24;
    }else{
        while( !waitpid(-1,&status,WNOHANG ) )
        {
            sleep( 3 );
            printf("sleep 3s.\n" );
        }
        printf("Child PID: %d \n",pid );
        wait( &status );
        if( WIFEXITED(status) )
            printf("Child send : %d \n",WEXITSTATUS(status) );
    }
    return 0;
}
/*
 * output:
    sleep 3s.
    sleep 3s.
    sleep 3s.
    sleep 3s.
    sleep 3s.
    sleep 3s.
    sleep 3s.
    sleep 3s.
    sleep 3s.
    sleep 3s.
    Child PID: 30775 
    Child send : 24
 * */
