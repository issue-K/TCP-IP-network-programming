#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>



#define TTL 64
#define BUF_SIZE 30
#define EPOLL_SIZE 50

void ERROR(char* message );

int main(){
    char buf[BUF_SIZE];

    struct sockaddr_in serv_adr, clnt_adr;
    int port = 9090;
    char* addr = "127.0.0.1";
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr( addr );
    serv_adr.sin_port = htons( port );

    int sock = socket( PF_INET,SOCK_STREAM, 0 );
    if( bind(sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr) ) == -1 )
        ERROR("bind() error." );
    if( listen(sock,5) == -1 )
        ERROR("listen() error." );
    int epfd = epoll_create(EPOLL_SIZE); //创建epoll 例程

    // 先注册服务端套接字
    struct epoll_event event;
    event.data.fd = sock;
    event.events = EPOLLIN;
    epoll_ctl( epfd,EPOLL_CTL_ADD,sock,&event );

    struct epoll_event* events = malloc( sizeof(struct epoll_event)*EPOLL_SIZE );
    while( 1 ){
        int cnt = epoll_wait( epfd,events ,EPOLL_SIZE,-1 );
        if( cnt == -1 )
            ERROR("epoll_wait() error." );
        int i;
        for(i = 0; i<cnt;i++ ){
            int fd = events[i].data.fd;
            if( fd == sock ){
                socklen_t adr_sz = sizeof( clnt_adr );
                int clnt_sock = accept(sock,(struct sockaddr*)&clnt_adr,&adr_sz);
                struct epoll_event event;
                event.data.fd = clnt_sock;
                event.events = EPOLLIN;
                epoll_ctl( epfd,EPOLL_CTL_ADD,clnt_sock,&event );
                printf("connected client: %d \n",clnt_sock );
            }else{ // 客户端套接
                int str_len = read( fd,buf,BUF_SIZE );
                if( str_len == 0 ){  // 断开连接了
                    epoll_ctl( epfd,EPOLL_CTL_DEL,fd,NULL );
                    close( fd );
                    printf("closed client: %d \n",fd );
                }else{
                    write( fd,buf,str_len );
                }
            }
        }
    }
    close( sock );
    close( epfd );
    return 0;
}

void ERROR(char* message){
    printf("%s \n",message );
    exit( 1 );
}
