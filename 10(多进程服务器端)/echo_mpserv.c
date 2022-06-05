/*
多进程服务器服务端
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void ERROR(char* message);

void read_childproc(int sig){
    int status;
    pid_t pid = waitpid( -1,&status,WNOHANG );
    printf("removed proc id: %d \n",pid );
}

void ERROR(char* message ){
    fputs(message,stderr );
    fputc('\n',stderr );
    exit( 1 );
}

int main(){
    char buf[BUF_SIZE];
    pid_t pid;
    socklen_t adr_sz;
    int str_len, state;

    struct sigaction act;
    act.sa_handler = read_childproc;
    act.sa_flags = 0;
    sigemptyset( &act.sa_mask );
    state = sigaction( SIGCHLD,&act,0 );

    int serv_sock, clnt_sock;
    int port = 9998;
    char* addr = "127.0.0.1";
    struct sockaddr_in serv_adr, clnt_adr;

    memset( &serv_adr,0,sizeof(serv_adr) );
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons( port );
    serv_adr.sin_addr.s_addr = inet_addr( addr );
    serv_sock = socket( PF_INET,SOCK_STREAM,0 );

    if( bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr) )==-1  )
        ERROR("bind() error." );
    if( listen(serv_sock,5) == -1 )
        ERROR("listen() error." );
    while( 1 ){
        adr_sz = sizeof( clnt_adr );
        clnt_sock = accept( serv_sock,(struct sockaddr*)&clnt_sock,&adr_sz );
        if( clnt_sock == -1 )
            continue;
        else
            puts("new client connected..." );
        pid = fork();
        if( pid == -1 ){  // 创建进程失败
            close( clnt_sock );
            continue;
        }else if( pid == 0 ){  // 进入子进程
            close( serv_sock ); // 关闭服务器套接字
            while( (str_len = read(clnt_sock,buf,BUF_SIZE)) != 0 )
                write( clnt_sock,buf,str_len );
            close( clnt_sock );
            puts("client disconnected ..." );
            return 0;
        }else{  //父进程
            close( clnt_sock );
        }
    }
}
