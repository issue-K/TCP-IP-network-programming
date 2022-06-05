#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

#define ERROR( x ) \
    do{            \
        printf("%s\n",x ); \
        exit(0);   \
    }while(0)

char buf[BUF_SIZE];

void write_sock(int sock,char* buf){
    while( 1 ) {
        printf("please enter your message( press 'Q' exit.): " );
        fgets( buf,BUF_SIZE, stdin );
        if( !strcmp(buf,"Q\n") ){  // 不发送数据了, 应该关闭套接字
            shutdown( sock,SHUT_WR );
            break;
        }
        write(sock, buf, sizeof(buf));
    }
}
void read_sock(int sock,char* buf){
    while( 1 ) {
        int str_len = read(sock, buf, BUF_SIZE);
        if( str_len == 0 )  break;
        buf[str_len] = '\0';
        printf("receive message: %s \n", buf);
    }
}

int main(){
    struct sockaddr_in serv_adr, clnt_adr;
    int port = 9998;
    char* addr = "127.0.0.1";
    serv_adr.sin_port = htons( port );
    serv_adr.sin_addr.s_addr = inet_addr( addr );
    serv_adr.sin_family = AF_INET;

    int sock = socket( PF_INET,SOCK_STREAM, 0 );
    if( connect(sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr) ) == -1  )
        ERROR("connect() error." );

    pid_t pid = fork();
    if( pid == 0 ){
        write_sock(sock,buf);
    }else{
        read_sock(sock,buf);
    }
    close( sock );
    return 0;
}
