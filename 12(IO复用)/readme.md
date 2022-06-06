使用 ```select``` 函数时可以将多个文件描述符集中到一起统一监视

```select```函数定义如下所示

```c
#include <sys/select.h>
#include <sys/time.h>

int select(int maxfd, fd_set *readset, fd_set *writeset,
           fd_set *exceptset, const struct timeval *timeout);
/*
成功时返回大于 0 的值，失败时返回 -1
maxfd: 监视对象文件描述符数量
readset: 将所有关注「是否存在待读取数据」的文件描述符注册到 fd_set 型变量，并传递其地址值。
writeset: 将所有关注「是否可传输无阻塞数据」的文件描述符注册到 fd_set 型变量，并传递其地址值。
exceptset: 将所有关注「是否发生异常」的文件描述符注册到 fd_set 型变量，并传递其地址值。
timeout: 调用 select 函数后，为防止陷入无限阻塞的状态，传递超时(time-out)信息
返回值: 发生错误时返回 -1,超时时返回0,。因发生关注的时间返回时，返回大于0的值，该值是发生事件的文件描述符数。
*/
```

### 设置文件描述符

也就是初始化```fd_set```结构体指针

```fd_set```实质上是一个```01```数组, 索引```i```位置表示文件描述符```i```是否应该被监视. (为$1$表示监视)

对```fd_set```的操作都用下列宏来完成

- `FD_ZERO(fd_set *fdset)`：将 ```fdset``` 变量所指的位全部初始化成```0```
- `FD_SET(int fd,fd_set *fdset)`：在参数 ```fdset``` 指向的变量中注册文件描述符 ```fd``` 的信息
- `FD_CLR(int fd,fd_set *fdset)`：从参数 ```fdset``` 指向的变量中清除文件描述符 ```fd``` 的信息
- `FD_ISSET(int fd,fd_set *fdset)`：若参数 ```fdset``` 指向的变量中包含文件描述符 ```fd``` 的信息，则返回「真」

 

### 设置监视范围和超时

#### 监视范围

```select```函数的第一个参数```maxfd```表示文件描述符的监视范围

因为判断是否需要监视某个文件描述符, 还是需要遍历```fd_set```数组, 而```maxfd```规定了遍历的最大值

由于创建文件描述符时, 数字是递增的, 所以只需要传入最晚创建的文件描述符加一即可(文件描述符从```0```起)

#### 超时

```select```函数的最后一个参数```timeout```表示超时时间

本来```select```函数只在有监视的文件描述符发生变化才返回, ```timeout```表示在未发生变化的状态下经过```timeout```的时间也会返回```0```

下面是该结构的定义

```c
struct timeval
{
   long tv_sec;		/* Seconds.  */
   long tv_usec;	/* Microseconds.  */
};
```

下面是一个调用```select```的例子```select.c```

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 1024

int main(){
    fd_set reads, temps;
    int result, str_len;
    char buf[BUF_SIZE];
    struct timeval timeout;

    FD_ZERO( &reads ); // 初始化为0
    FD_SET( 0,&reads ); // 将文件描述符0对应位置为1
    while( 1 ){
        temps = reads; // 调用select后对应位的内容可能会变化, 存一下
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        result = select( 1,&temps,0,0,&timeout );
        if( result == -1 ){
            puts("select error!" );
            break;
        }else if( result == 0 ){
            puts("Time out!" );
        }else{
            if( FD_ISSET(0,&temps) ){  // 验证发生变化的位是否是输入端
                str_len = read( 0,buf,BUF_SIZE );
                buf[str_len] = '\0';
                printf("message from console: %s\n",buf );
            }
        }
    }
}
```



### 基于```select```的服务器

代码```echo_select_server.c```

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 1024
void ERROR(char* message);

int main(){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    struct timeval timeout;
    fd_set reads, cpy_reads;
    socklen_t adr_sz;
    int fd_max, str_len, fd_num, i;

    char buf[BUF_SIZE];
    char* addr = "127.0.0.1";
    int port = 9091;
    serv_sock = socket( PF_INET,SOCK_STREAM, 0 );
    memset( &serv_adr,0,sizeof(serv_adr) );
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons( port );
    serv_adr.sin_addr.s_addr = inet_addr(addr );

    if( bind(serv_sock,(struct sockaddr*)&serv_adr, sizeof(serv_adr) ) == -1 )
        ERROR( "bind() error" );
    if( listen(serv_sock,5 ) == -1 )
        ERROR("listen() error" );


    FD_ZERO(&reads);
    FD_SET( serv_sock,&reads );
    fd_max = serv_sock;
    while( 1 ){
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;
        if( (fd_num = select(fd_max+1,&cpy_reads,0,0,&timeout) ) == -1 )            break;
        if( fd_num == 0 )
            continue;
        int i;
        for( i=0;i<fd_max+1;i++){
            if( FD_ISSET(i,&cpy_reads) ){
                if( i == serv_sock ) {  // 服务端套接字
                    adr_sz = sizeof( clnt_adr );
                    clnt_sock = accept( serv_sock,(struct sockaddr*)&clnt_adr,&adr_sz );
                    FD_SET( clnt_sock,&reads ); // 注册一个clnt_sock监视
                    if( fd_max < clnt_sock )
                        fd_max = clnt_sock;
                    printf("Connect client: %d\n",clnt_sock );
                }else{ // 客户端套接字(连接套接字)
                    str_len = read( i,buf,BUF_SIZE );
                    if( str_len == 0 ){
                        FD_CLR( i,&reads ); // 清除该套接字
                        close( i );
                        printf("closed client: %d \n", i );
                    }else{
                        write( i,buf,str_len );
                    }
                }
            }
        }
    }
    close( serv_sock );
    return 0;
}

void ERROR(char* message){
    fputs(message, stderr );
    fputs('\n', stderr );
    exit( 1 );
}
```

