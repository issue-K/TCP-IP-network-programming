
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

    if( bind(sock,(struct sockaddr*)&serv_adr, sizeof(serv_adr) )==-1  )
        ERROR("bind() error." );
    if( listen(sock,5)==-1 )
        ERROR("listen() error." );

    int cl_sock = accept(sock,(struct sockaddr*)&clnt_adr, &clnt_adr_sz );
    // 开始读取文件发送数据
    FILE *p = fopen( "/tmp/tmp.8L0S0EyXlm/data.txt","rb");
    if( !p )
        ERROR( "fopen() error." );
    printf("创建套接字成功,sock = %d\n",cl_sock );
    char message[BUF_SIZE];
    while( 1 ) {
        int read_len = fread(message, 1, BUF_SIZE, p);
        if( read_len < BUF_SIZE ){ // read complete
            write( cl_sock ,message,read_len);
            printf("send message : %s\n",message );
            break;
        }
        write( cl_sock,message,read_len );
    }
    shutdown( cl_sock, SHUT_WR ); // 不需要发送数据了,断开输出流
    read( cl_sock,message,BUF_SIZE ); // 接收客户端继续发送的数据
    printf("receive client message: %s",message );
    fclose( p );
    close( sock );
    close( cl_sock );
    return 0;
}
