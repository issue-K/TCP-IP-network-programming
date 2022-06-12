#### select的缺点

```select```有很多缺点,

- 调用```select```函数后, 需要对返回的```fd_set```进行循环扫描才能知道哪些文件描述符有消息
- 每次调用```select```函数需要向该函数传递监视对象的信息

每次调用```selecet```进行的循环是```O(n)```的, 对性能影响已经很大

然而更大的障碍在于每次都需要传递监视对象的值(```fd_set```), 这给操作系统带来很大的负担

有些函数不需要操作系统就能完成, 有些则必须借助操作系统

### ```epoll```的优点

- 无需编写以监视状态发生变化为目的的针对所有文件描述符的循环语句(```epoll```直接返回状态变化的文件描述符)
- 调用```epoll_wait```函数时不需要每次都传入监视对象信息(省去很多操作系统的工作)

下面介绍在```epoll```服务器端实现中需要的三个函数

- ```epoll_create```: 创建保存```epoll```文件描述符的空间
- ```epoll_ctl```: 向空间注册并注销文件描述符
- ```epoll_wait```: 与```select```函数类似, 等待文件描述符发生变化

#### ```epoll_create```

```select```函数中直接使用```fd_set```变量保存需要监视的文件描述符

但```epoll```方式下直接由操作系统保存需要监视的对象, 所以需要在操作系统中请求创建保存文件描述符的空间

函数原型

```
int epoll_create(int size);
```

- ```size```: 调用```epoll_create```创建的文件描述符保存空间叫```epoll```例程, ```size```参数决定着```epoll```例程的大小(仅供操作系统参考)

#### ```epoll_ctl```

生成```epoll```例程后, 应在内部注册监视对象文件描述符, 使用```epoll_ctl```函数

```c
int epoll_ctl(int epfd,int op,int fd,struct epoll_event* event)
成功返回0, 失败返回-1
```

- ```epfd```：用于注册监视对象的 ```epoll``` 例程的文件描述符

- ```op```：用于制定监视对象的添加、删除或更改等操作. 它的取值为下列宏之一

  ```EPOLL_CTL_ADD```：将文件描述符注册到 ```epoll``` 例程

  ```EPOLL_CTL_DEL```：从 ```epoll``` 例程中删除文件描述符

  ```EPOLL_CTL_MOD```：更改注册的文件描述符的关注事件发生情况

- ```fd```：需要注册的监视对象文件描述符

- ```event```：监视对象的事件类型. ```epoll_event```的定义如下所示

  ```c
  struct epoll_event
  {
      __uint32_t events;
      epoll_data_t data;
  };
  typedef union epoll_data {
      void *ptr;
      int fd;
      __uint32_t u32;
      __uint64_t u64;
  } epoll_data_t;
  ```

  其中```events```表示事件类型, 它的取值可以是以下宏的子集

  ```c
  EPOLLIN：需要读取数据的情况
  EPOLLOUT：输出缓冲为空，可以立即发送数据的情况
  EPOLLPRI：收到 OOB 数据的情况
  EPOLLRDHUP：断开连接或半关闭的情况，这在边缘触发方式下非常有用
  EPOLLERR：发生错误的情况
  EPOLLET：以边缘触发的方式得到事件通知
  EPOLLONESHOT：发生一次事件后，相应文件描述符不再收到事件通知。因此需要向 epoll_ctl 函数的第二个参数传递 EPOLL_CTL_MOD ，再次设置事件。
  ```

  

这样看还是优点难以理解, 看一个使用```epoll_ctl```的例子

```c
struct epoll_event event;
event.events=EPOLLIN;//发生需要读取数据的情况时
event.data.fd=sockfd;
epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&event);
```

### ```epoll_wait```

函数原型为

```c
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
/*
成功时返回发生事件的文件描述符，失败时返回 -1
epfd : 表示事件发生监视范围的 epoll 例程的文件描述符
events : 保存发生事件的文件描述符集合的结构体地址值
maxevents : 第二个参数中可以保存的最大事件数
timeout : 以 1/1000 秒为单位的等待时间，传递 -1 时，一直等待直到发生事件
*/
```

需要注意的是第二个参数需要动态分配空间

```c
int event_cnt;
struct epoll_event *ep_events;
...
ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);//EPOLL_SIZE是宏常量
...
event_cnt=epoll_wait(epfd,ep_events,EPOLL_SIZE,-1);
...
```

调用函数后, 会在第二个参数指向的缓冲保存发生事件的文件描述符集合，省去了```select```循环的过程



### 基于```epoll```实现的回声服务器端

服务端代码在```epoll_server.c```中, 客户端无需变化, 可以直接使用第四章的回声客户端, 代码和```epoll_client.c```一致

下面可以仔细看一下```epoll_server.c```的代码

```c
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
```

总结一下``` epoll ```的流程：

1. ```epoll_create``` 创建一个保存 ```epoll``` 文件描述符的空间
2. 动态分配内存，给将要监视的 ```epoll_wait```
3. 利用 ```epoll_ctl``` 控制 添加 删除需要监听的文件描述符
4. 利用 ```epoll_wait``` 来获取改变的文件描述符,来执行程序

### 条件触发和边缘触发

**条件触发**

```epoll```默认是条件触发

 只要输入缓冲有数据就会一直通知该事件

比如当输入缓冲收到```50```字节时, 服务端操作系统会通知该事件. 但若只读取其中```20```字节, 在下次调用```epoll_wait```时仍然会返回该文件描述符.这个验证起来很简单, 只需要把上面```epoll_server.c```的```BUF_SIZE```调小一点导致一次读不完, 可以发现消息仍然能发送出去(经过多次```epoll_wait```分几次发完)



**边缘触发**

每当输入缓冲新来数据时, 会通知一次.

至于读不读完是你的事, 所以如果你读的比较慢可能导致你读的数据是之前很早发过来的

设置边缘触发很简单

```c
 event.events = EPOLLIN | EPOLLET; // EPOLLET表示边缘触发
```

