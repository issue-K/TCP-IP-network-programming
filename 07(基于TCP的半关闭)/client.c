
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

int main(){
    int port = 9091;
    int sock = socket( PF_INET,SOCK_STREAM, 0 );
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons( port );
    serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");


    if ( connect( sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr) ) == -1 )
        ERROR("connect() error." );
    char message[BUF_SIZE];
    FILE* p = fopen("/tmp/tmp.hE92hjpMCX/receivedata.txt","wb" );
    if( !p )
        ERROR("fopen() error" );
    while( 1 )
    {
        int read_len = read(sock,message,BUF_SIZE );
        if( read_len == 0 )     break;
        fwrite(message,1,read_len,p );
    }
    shutdown( sock,SHUT_RD );  // 不需要读数据了
    strcpy( message, "message complete, thank!" );
    write( sock,message,sizeof(message) );
    close( sock );
    return 0;
}
