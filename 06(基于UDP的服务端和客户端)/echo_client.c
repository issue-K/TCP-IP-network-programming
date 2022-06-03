#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

#define BUF_SIZE 1024
#define ERROR(x) \
    do{          \
        printf("%s\n",x ); \
        return 0;         \
    }while(0)

#define TRUE 1

int main(){
    int sock;
    char message[BUF_SIZE];
    int port = 9090;
    struct sockaddr_in serv_adr, from_adr;
    // 创建UDP套接字
    sock =  socket( PF_INET,SOCK_DGRAM,0 );
    if( sock == -1 )
        ERROR("socket create error" );
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons( port );
    serv_adr.sin_addr.s_addr = htonl( INADDR_ANY );
    while( 1 )
    {
        fputs("Insert your message( enter Q exit program ): ",stdout );
        fgets( message, sizeof(message), stdin );
        if( !strcmp(message,"Q\n") )    break;
        sendto( sock,message,strlen(message),0, (struct sockaddr*)&serv_adr,sizeof(serv_adr) );
        socklen_t addr_len = sizeof( from_adr );
        int str_len = recvfrom( sock,message,BUF_SIZE,0,(struct sockaddr*)&from_adr,&addr_len );  // from_adr是发送方的地址
        message[str_len] = '\0';
        printf("receive server message: %s ",message );
    }
    close( sock ) ;
    return 0;
}
