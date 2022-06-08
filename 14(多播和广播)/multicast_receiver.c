#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void ERROR(char *message);

const int port = 9091;
char buf[BUF_SIZE];

int main()
{
    int str_len;
    char buf[BUF_SIZE];
    struct sockaddr_in adr;

    int recv_sock = socket( PF_INET,SOCK_DGRAM,0 );
    memset( &adr,0,sizeof(adr) );
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl( INADDR_ANY );
    adr.sin_port = htons( port );

    if( bind(recv_sock,(struct sockaddr*)&adr,sizeof(adr) ) == -1 )
        ERROR("bind() error.");

    struct ip_mreq join_adr;
    char* multi_adr = "224.1.1.2";
    join_adr.imr_multiaddr.s_addr = inet_addr( multi_adr ); // 多播组地址
    join_adr.imr_interface.s_addr = htonl( INADDR_ANY ); // 待加入的ip地址
    setsockopt( recv_sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,(void*)&join_adr,sizeof(join_adr));

    while( 1 )
    {
        printf("enter......\n" );
        str_len = recvfrom( recv_sock,buf,BUF_SIZE-1,0,NULL,0 );
        if( str_len < 0 ) break;
        buf[str_len] = 0;
        fputs( buf,stdout );
    }
    close( recv_sock );
    return 0;
}

void ERROR(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
