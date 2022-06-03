
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
    int port = 9090;
    int serv_sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t clnt_adr_sz;
    struct sockaddr_in serv_adr, clnt_adr;
    // 创建UDP套接字
    serv_sock = socket( PF_INET,SOCK_DGRAM,0 );
    if( serv_sock == -1 )
        ERROR("UDP socket create error" );
    memset( &serv_adr,0,sizeof(serv_adr ) );
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY );
    serv_adr.sin_port = htons( port );
    // 分配地址接收数据, 不限制数据传输对象
    if( bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr) )==-1 )
        ERROR("bind() error" );
    while( 1 )
    {
        clnt_adr_sz = sizeof( clnt_adr );
        str_len = recvfrom( serv_sock,message,BUF_SIZE,0,(struct sockaddr*)&clnt_adr,&clnt_adr_sz );
        message[str_len] = '>';  // 表示为服务器返回的数据特征
        message[str_len+1] = '\0';
        sendto( serv_sock,message,str_len+1,0,(struct sockaddr*)&clnt_adr,clnt_adr_sz );
    }
    close( serv_sock );
    return 0;
}
