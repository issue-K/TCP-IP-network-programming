#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void timeout(int sig){
    if( sig == SIGCHLD ){
        int status;
        pid_t pid = waitpid( -1,&status,WNOHANG );
        if( WIFEXITED(status) ){
            printf("Remove proc id: %d\n", pid );
            printf("Child send: %d \n", WEXITSTATUS(status) );
        }
    }
}

int main(){

    int i;
    struct sigaction act;
    act.sa_handler = timeout;
    sigemptyset( &act.sa_mask );
    act.sa_flags = 0;

    sigaction( SIGCHLD,&act, 0 );

    pid_t pid = fork();
    if( pid == 0 ){  // 子进程执行区域
        return 12;
    }else{
        printf("Child proc id: %d\n",pid );
        for( i=0;i<4;i++){
            sleep( 5 );
        }
    }

    return 0;
}
/*
 *  output:
    Child proc id: 7668
    Remove proc id: 7668
    Child send: 12 
 */
