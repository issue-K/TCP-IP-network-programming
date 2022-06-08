#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TTL 64
#define BUF_SIZE 30
void ERROR(char* message );

int main(){
    int send_sock;
    struct sockaddr_in mul_adr;
    int time_live = TTL;

    char* addr = "224.1.1.2";
    int port = 9091;
    send_sock = socket( PF_INET,SOCK_DGRAM,0 );
    memset( &mul_adr,0,sizeof( mul_adr) );
    mul_adr.sin_family = AF_INET;
    mul_adr.sin_addr.s_addr = inet_addr( addr );
    mul_adr.sin_port = htons( port );
    // 指定套接字的TTL信息
    setsockopt( send_sock,IPPROTO_IP,IP_MULTICAST_TTL,(void*)&time_live,sizeof(time_live) );
    for(int i=1;i<=10;i++){
        char buf[BUF_SIZE] = "asdasd";
        sendto( send_sock,buf,strlen(buf),0,(struct sockaddr*)&mul_adr,sizeof(mul_adr));
        sleep( 2 );
        printf("发完一个\n" );
    }
    close( send_sock );
    return 0;
}
