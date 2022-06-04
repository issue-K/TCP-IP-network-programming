
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 1024

#define ERROR( x ) \
    do{            \
        printf("%s\n",x ); \
        exit(0);   \
    }while(0)

int main(){
    char* domain = "www.baidu.com";
    struct hostent *host = gethostbyname( domain );
    if( !host )
        ERROR( "gethostbyname() error." );
    // 官方域名
    printf("Official name: %s \n",host->h_name );
    // cname域名
    int i;
    for( i = 0; host->h_aliases[i];i++ )
        printf("Aliases %d: %s \n", i+1, host->h_aliases[i] );
    printf("Address type: %s \n",( host->h_addrtype == AF_INET )?"AF_INET":"AF_INET6" );
    for( i = 0; host->h_addr_list[i];i++ ) {
        printf(" IP addr %d: %s \n", i + 1,
               inet_ntoa(*(struct in_addr *) host->h_addr_list[i]));
    }
    return 0;
}
