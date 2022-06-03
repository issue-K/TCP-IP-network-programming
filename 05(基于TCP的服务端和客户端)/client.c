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
    u_int16_t port = 9091;
    struct sockaddr_in serv_addr, clnt_addr;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(port);
    serv_addr.sin_family = AF_INET;

    int sock = socket(PF_INET,SOCK_STREAM,0 );
    if( connect(sock,(struct sockaddr*)(&serv_addr),sizeof(serv_addr))==-1 )
    {
        ERROR("connect() error");
    }
    fputs("operator count: ",stdout );
    int count, i;
    scanf("%d",&count );
    if( write( sock,&count,sizeof(count) ) == -1 )
        ERROR( "33 write() error" );
    for( i=0;i<count;i++ ){
        char* s = (char *)malloc( 100 );
        sprintf( s, "operator %d is: ", i+1 );
        fputs( s,stdout  );
        int num;
        scanf("%d",&num );
        write( sock,&num,sizeof(num) );
    }
    fputs("select one of +-x/: ",stdout );
    fgetc( stdin );
    char c;
    scanf("%c",&c );
//    c = fgetc( stdin );
    write( sock,&c,sizeof(c) );
    int ans;
    read( sock,&ans,sizeof(ans) );
    printf("server return answer : %d\n",ans );
    close( sock );
    return 0;
}
