### fork函数

```fork```函数用于在程序中新建一个进程, 并返回子进程的```ID```(失败返回```-1```)

```c
pid_t fork(void)
```

子进程会完全复制父进程的一切内容, 但是在子进程中```fork()```函数的返回值是```0```, 这也是区分父进程和子进程的一个标识.

两个进程都会执行```fork```返回后的代码

接下来给出一个简单的例子, 你可以在```fork.c```找到代码.

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int gval = 10;

int main(){
    pid_t pid;
    int lval = 20;
    gval++, lval += 5;
    pid = fork();
    if( pid ){
        gval -=  2, lval -= 2;
    }else{
        gval += 2, lval += 2;
    }
    if( pid==0 ){
        printf("Child proc:[%d %d]",gval,lval );
    }else{
        printf("Parent proc: [%d %d] \n",gval,lval );
    }
    return 0;
}

/*
output: 
Parent proc: [9 23] 
Child proc:[13 27]
*/
```



###   僵尸进程

僵尸进程是当子进程比父进程先结束，而父进程又没有回收子进程，释放子进程占用的资源，此时子进程将成为一个僵尸进程。如果父进程先退出 ，子进程被```init```接管，子进程退出后```init```会回收其占用的相关资源

进程向```exit```传递的参数值和```main```函数的```return```语句的返回值都会传递给操作系统, 而操作系统不会销毁子进程

直到把这些值传递给产生该子进程的父进程

如何向父进程传递这些值呢?只有父进程主动发起请求时, 操作系统才会传递这些值

- 一个僵尸进程的例子(由于父进程```sleep(30)```,在```30s```内父进程不会结束, 子进程不会被```init```进程回收, 此时是僵尸进程)

  ```
  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  
  
  int main(){
      pid_t pid = fork();
      if( pid == 0 ){
          puts("Hi, I am child Process." );
      }else{
          printf("Child Process ID: %d \n",pid );
          sleep( 30 );
      }
      if( pid == 0)
          printf("End child process\n" );
      else
          printf("End parent process\n" );
      return 0;
  }
  ```

输入命令```ps -aux```查看在运行的进程, 可以找到这个僵尸进程(```30s```内)

![image-20220605094126569](C:\Users\86189\AppData\Roaming\Typora\typora-user-images\image-20220605094126569.png)

### 销毁僵尸进程

#### 一、 使用```wait```函数

为了销毁子进程, 父进程应主动请求获取子进程的返回值.

```c
pid_t wait(int* statloc)
```

成功时返回终止的子进程```PID```, 失败则返回```-1```

如果在调用```wait```时没有已经终止的子进程, **程序将被阻塞**。

子进程终止时传递的返回值将保存在函数参数所指的内存空间.当该内存空间还包含其他信息, 因此需要通过以下宏来分离

```c
WIFEXITED 子进程正常终止返回 true 
WEXITSTATUS  返回子进程的返回值
```

一个销毁子进程的例子```wait.c```

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(){
    int status;
    pid_t pid = fork();
    if( pid == 0 ){
        return 3;
    }else{
        printf("Child1 PID: %d \n",pid );
        pid = fork();
        if( pid==0 ){
            exit( 7 );
        }else{
            // 父进程需要同时回收上面两个子进程, 所以调用两次wait 
            printf("Child2 PID: %d \n",pid );
            wait( &status );
            if( WIFEXITED(status) )
                printf("Child send two: %d \n",WEXITSTATUS(status) );
            wait( &status );
            if( WIFEXITED(status) )
                printf("Child send two: %d \n",WEXITSTATUS(status ) );
            sleep( 30 );
        }
    }
    return 0;
}
/*
 * output:
    Child1 PID: 28722
    Child2 PID: 28723 
    Child send two: 3
    Child send two: 7
 * */
```

#### 二、使用```waitpid```函数

```wait```会引起程序阻塞, 可以考虑使用```waitpid```函数

```c
pid_t waitpid(pid_t pid,int* statloc,int options)
```

成功时返回终止的子进程```PID```, 失败返回```-1```

```pid```: 等待终止的目标子进程```pid```, 若为```-1```可以等待任意子进程终止

```statloc```: 与```wait```函数相同

```options```: 传递头文件```sys/wait.h```中声明的常量```WNOHANG```, 即使没有终止的子进程也不会进入阻塞状态, 而是返回```0```并退出函数

一个例子```waitpid.c```

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(){
    int status;
    pid_t pid = fork();
    if( pid == 0 ){
        sleep( 30 );
        return 24;
    }else{
        while( !waitpid(-1,&status,WNOHANG ) )
        {
            sleep( 3 );
            printf("sleep 3s.\n" );
        }
        printf("Child PID: %d \n",pid );
        wait( &status );
        if( WIFEXITED(status) )
            printf("Child send : %d \n",WEXITSTATUS(status) );
    }
    return 0;
}
/*
 * output:
    sleep 3s.
    sleep 3s.
    sleep 3s.
    sleep 3s.
    sleep 3s.
    sleep 3s.
    sleep 3s.
    sleep 3s.
    sleep 3s.
    sleep 3s.
    Child PID: 30775 
    Child send : 24
 * */
```

### 信号处理

我们已经知道了进程的创建和销毁方法, 但是目前

#### 利用```signal```函数进行信号处理

下面是参数```signo```的一些宏的意义:

- ```SIGALRM```: 已经通过调用```alarm```函数注册的时间
- ```SIGINT```：输入```CTRL+C```
- ```SIGCHLD```: 子进程终止

再来介绍以下```alarm```函数

```c
unsigned int alarm(unsigned int seconds);
```

给该函数传递一个正整数参数, 在相应的秒数后将产生```SIGALRM```信号。若传递参数```0```, 那么会取消之前对```SIGALRM```信号的预约

一个例子```signal.c```

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
void timeout(int sig) //信号处理器
{
    if (sig == SIGALRM)
        puts("Time out!");
    alarm(2); //为了每隔 2 秒重复产生 SIGALRM 信号，在信号处理器中调用 alarm 函数
}
void keycontrol(int sig) //信号处理器
{
    if (sig == SIGINT)
        puts("CTRL+C pressed");
}
int main(int argc, char *argv[])
{
    int i;
    signal(SIGALRM, timeout); //注册信号及相应处理器
    signal(SIGINT, keycontrol);
    alarm(2); //预约 2 秒候发生 SIGALRM 信号

    for (i = 0; i < 3; i++)
    {
        puts("wait...");
        sleep(100);  // sleep在收到信号后会立刻被唤醒
    }
    return 0;
}
/*
 * output:
    wait...
    Time out!
    wait...
    Time out!
    wait...
    Time out!
 * */
```

#### 利用```sigaction```函数进行信号处理

前面的内容已足以用于编写防止僵尸进程生成的代码, 但是还是需要介绍以下```sigaction```

它功能和```signal```类似, 但更加稳定, 在各个```UNIX```系列的操作系统中完全相同

```c
int sigaction(int signo, const sigaction *act, sigaction *oldact)
```

成功返回$0$, 失败返回$1$

- ```signo```: 传递信号信息
- ```act```: 对应第一个参数的信号处理函数
- ```oldact```: 通过该参数获取之前注册的信号处理函数指针, 不需要传递$0$

```sigaction```结构体定义如下所示

```c
struct sigaction
{
	void (*sa_handler)(int);
    /* Additional set of signals to be blocked.  */
    __sigset_t sa_mask;

    /* Special flags.  */
    int sa_flags;
};
```

现在只需要关注```sa_handler```成员是保存信号处理函数的地址值即可.

一个例子```sigaction.c```

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig){
    if( sig == SIGALRM )
        puts("Time out!" );
    alarm( 2 );
}

int main(){
    int i;
    struct sigaction act;
    act.sa_handler = timeout;
    sigemptyset( &act.sa_mask );
    act.sa_flags = 0;

    sigaction( SIGALRM,&act, 0 );
    alarm( 2 );
    for( i = 0 ;i < 3; i++){
        puts( "wait ... " );
        sleep( 100 );
    }
    return 0;
}
```

### 利用信号处理技术消灭僵尸进程

这个就比较简单了, 因为子进程终止时会产生信号```SIGCHLD```

父进程只需要用```sigaction```函数把该信号值和对应的信号处理函数(也就是处理僵尸进程函数,调用```wait```)绑定起来即可

这里也给出一个参考```remove_zombie.c```

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void timeout(int sig){
    if( sig == SIGCHLD ){
        int status;
        pid_t pid = waitpid( -1,&status,WNOHANG );
        if( WIFEXITED(status) ){
            printf("Remove proc id: %d\n", pid );
            printf("Child send: %d \n", WEXITSTATUS(status) );
        }
    }
}

int main(){

    int i;
    struct sigaction act;
    act.sa_handler = timeout;
    sigemptyset( &act.sa_mask );
    act.sa_flags = 0;

    sigaction( SIGCHLD,&act, 0 );

    pid_t pid = fork();
    if( pid == 0 ){  // 子进程执行区域
        return 12;
    }else{
        printf("Child proc id: %d\n",pid );
        for( i=0;i<4;i++){
            sleep( 5 );
        }
    }

    return 0;
}
/*
 *  output:
    Child proc id: 7668
    Remove proc id: 7668
    Child send: 12 
 */
```



### 基于多任务的并发服务器

好了, 做了这么多铺垫之后, 终于可以开始编写像样的服务器了.

这里给出一个基于```fork```的多进程回声服务器```echo_serv.c```

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void ERROR(char* message);

void read_childproc(int sig){
    int status;
    pid_t pid = waitpid( -1,&status,WNOHANG );
    printf("removed proc id: %d \n",pid );
}

void ERROR(char* message ){
    fputs(message,stderr );
    fputc('\n',stderr );
    exit( 1 );
}

int main(){
    char buf[BUF_SIZE];
    pid_t pid;
    socklen_t adr_sz;
    int str_len, state;

    struct sigaction act;
    act.sa_handler = read_childproc;
    act.sa_flags = 0;
    sigemptyset( &act.sa_mask );
    state = sigaction( SIGCHLD,&act,0 );

    int serv_sock, clnt_sock;
    int port = 9998;
    char* addr = "127.0.0.1";
    struct sockaddr_in serv_adr, clnt_adr;

    memset( &serv_adr,0,sizeof(serv_adr) );
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons( port );
    serv_adr.sin_addr.s_addr = inet_addr( addr );
    serv_sock = socket( PF_INET,SOCK_STREAM,0 );

    if( bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr) )==-1  )
        ERROR("bind() error." );
    if( listen(serv_sock,5) == -1 )
        ERROR("listen() error." );
    while( 1 ){
        adr_sz = sizeof( clnt_adr );
        clnt_sock = accept( serv_sock,(struct sockaddr*)&clnt_sock,&adr_sz );
        if( clnt_sock == -1 )
            continue;
        else
            puts("new client connected..." );
        pid = fork();
        if( pid == -1 ){  // 创建进程失败
            close( clnt_sock );
            continue;
        }else if( pid == 0 ){  // 进入子进程
            close( serv_sock ); // 关闭服务器套接字
            while( (str_len = read(clnt_sock,buf,BUF_SIZE)) != 0 )
                write( clnt_sock,buf,str_len );
            close( clnt_sock );
            puts("client disconnected ..." );
            return 0;
        }else{  //父进程
            close( clnt_sock );
        }
    }
}
```

值得一提的是, 在父进程通过```accept```函数接收一个连接请求创建客户端套接字此时会开一个子进程对该套接字进行处理

子进程会复制父进程的所有资源, 也就是会把套接字的文件描述符复制过去.

注意,并不是复制套接字, 套接字和端口是一一对应的, 所以复制套接字也不合理, 只是父进程和子进程的文件描述符指向同一个套接字

而且, 当一个套接字存在多个文件描述符时, 需要所有的文件描述符都被销毁套接字才会被销毁

所以即使在子进程也需要释放服务端套接字(第```64```行)

该程序对应的客户端代码在```echo_client.c```中, 这里就不展示了, 和之前的普通回声客户端完全相同.

### 补充

虽然客户端代码可以直接使用之前写的普通回声客户端, 但现在我们可以使用多进程, 能不能优化以下呢?

之前的逻辑是, 客户端调用```write```发送数据后, 再调用```read```读取服务端的回声数据.

是一个串行执行的过程, 我们可以使用```fork```建一个子线程

让父进程只负责写(```write```), 子进程只负责读(```read```)

这样```write```就不需要等上次```read```结束才能调用

一个例子```echo_client_fork.c```

```c
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

char buf[BUF_SIZE];

void write_sock(int sock,char* buf){
    while( 1 ) {
        printf("please enter your message( press 'Q' exit.): " );
        fgets( buf,BUF_SIZE, stdin );
        if( !strcmp(buf,"Q\n") ){  // 不发送数据了, 应该关闭套接字
            shutdown( sock,SHUT_WR );
            break;
        }
        write(sock, buf, sizeof(buf));
    }
}
void read_sock(int sock,char* buf){
    while( 1 ) {
        int str_len = read(sock, buf, BUF_SIZE);
        if( str_len == 0 )  break;
        buf[str_len] = '\0';
        printf("receive message: %s \n", buf);
    }
}

int main(){
    struct sockaddr_in serv_adr, clnt_adr;
    int port = 9998;
    char* addr = "127.0.0.1";
    serv_adr.sin_port = htons( port );
    serv_adr.sin_addr.s_addr = inet_addr( addr );
    serv_adr.sin_family = AF_INET;

    int sock = socket( PF_INET,SOCK_STREAM, 0 );
    if( connect(sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr) ) == -1  )
        ERROR("connect() error." );

    pid_t pid = fork();
    if( pid == 0 ){
        write_sock(sock,buf);
    }else{
        read_sock(sock,buf);
    }
    close( sock );
    return 0;
}
```

注意这里由子进程负责写数据

```c
void write_sock(int sock,char* buf){
    while( 1 ) {
        printf("please enter your message( press 'Q' exit.): " );
        fgets( buf,BUF_SIZE, stdin );
        if( !strcmp(buf,"Q\n") ){  // 不发送数据了, 应该关闭套接字
            shutdown( sock,SHUT_WR );
            break;
        }
        write(sock, buf, sizeof(buf));
    }
}
```

可以看到当发送完全部数据后, 我们应该关闭套接字来告诉服务端没有数据了, 此前我们的做法是直接```close(sock)```

然而这是在子进程中, 只有父子进程都调用```close(sock)```时套接字才会真正被释放, 所以这里使用```shutdown```单独关闭输出流
