#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZ 1024

#define ERROR( x ) \
    do{            \
        printf("%s\n",x ); \
        exit(0);   \
    }while(0)

int main(){
    int fd[2];
    char buf[BUF_SIZ] = "i am fucking king";
    int ok = pipe( fd );
    if( ok == -1 )
        ERROR("pipe() error.");
    pid_t pid = fork();
    if( pid == 0 ){  // 子进程, 发送数据
        write( fd[1],buf,BUF_SIZ );
    }else{  // 父进程, 接收数据
        int str_len = read( fd[0],buf,BUF_SIZ );
        buf[str_len] = '\0';
        printf("parent proc receive message: %s\n",buf );
    }
}
/*
 * output:
   parent proc receive message: i am fucking king
 */
