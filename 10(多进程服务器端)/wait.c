
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(){
    int status;
    pid_t pid = fork();
    if( pid == 0 ){
        return 3;
    }else{
        printf("Child1 PID: %d \n",pid );
        pid = fork();
        if( pid==0 ){
            exit( 7 );
        }else{
            // 父进程需要同时回收上面两个子进程, 所以调用两次wait 
            printf("Child2 PID: %d \n",pid );
            wait( &status );
            if( WIFEXITED(status) )
                printf("Child send two: %d \n",WEXITSTATUS(status) );
            wait( &status );
            if( WIFEXITED(status) )
                printf("Child send two: %d \n",WEXITSTATUS(status ) );
            sleep( 30 );
        }
    }
    return 0;
}
/*
 * output:
    Child1 PID: 28722
    Child2 PID: 28723 
    Child send two: 3
    Child send two: 7
 * */
