
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

u_int16_t port = 9091;
int serv_sock, clnt_sock;
char message[BUF_SIZE];
int str_len, i;
struct sockaddr_in serv_addr, clnt_addr;
socklen_t clnt_addr_siz = sizeof( clnt_addr );

void huisheng_serve(){
    for( i=0;i<5;i++ ){
        printf("waiting......\n" );
        clnt_sock = accept( serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_siz);
        if( clnt_sock == -1 )
            ERROR("accept() error" );
        else
            printf("Connect client %d \n",i+1 );
        while( (str_len = read(clnt_sock,message,BUF_SIZE))!=0 )
            write( clnt_sock,message,str_len );
        close( clnt_sock );
    }
}
void calculate_serve(){
    for( i=0;i<5;i++ ){
        printf("waiting client %d\n", i  );
        clnt_sock = accept( serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_siz);
        if( clnt_sock == -1 )
            ERROR("accept() error" );
        // 默认第一个信息为: 数字的个数
        int num;
        read( clnt_sock,&num,sizeof(num) );  // 不考虑用户传来的num<=0 || num>=5
        int nums[5];
        for(int j=0;j<num;j++ ){
            int val;
            read( clnt_sock,&val,sizeof( val ) );
            printf("收到数据 %d\n", val );
            nums[j] = val;
        }
        char c;
        read( clnt_sock,&c,sizeof( c ) );
        int ans = nums[0];
        if( c=='+' ){
            for(int j=1;j<num;j++ ) ans += nums[j];
        }
        write( clnt_sock, &ans,sizeof( ans ) );
        close( clnt_sock );
    }
}

int main(){
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    serv_addr.sin_port = htons( port );

    serv_sock = socket( PF_INET,SOCK_STREAM,0 );
    if( serv_sock == -1 )
        ERROR( "socket() error" );
    if( bind(serv_sock, (struct sockaddr*)&serv_addr,sizeof(serv_addr) )==-1 )
        ERROR("bind() error") ;
    if( listen(serv_sock,5)==-1  )
        ERROR("listen() error" );
    calculate_serve();
    close( serv_sock );
    return 0;
}
