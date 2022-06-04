### UDP的服务端和客户端无连接

由于UDP不需要像TCP一样进行连接(三次握手,ack,seq等机制), 也就不需要调用```listen```和```accept```函数

UDP只有创建套接字的过程和数据交换的过程

### UDP的服务端和客户端均只需要一个套接字

TCP中, 套接字间是一一对应的关系(通过```accept```函数和```connect```函数)

但在UDP中, 只需要一个UDP套接字即可向任意主机传输数据

### 基于UDP的数据I/O函数

**发送函数**

```c
ssize_t sendto(int fd, const void *buf, size_t n, int flags, sockaddr *addr, socklen_t addr_len)    
```

```sock```:  用于传输数据的UDP套接字文件描述符

```buff```: 保存传输数据的缓存地址值

```n```: 传输数据的大小(字节为单位)

```addr```: 目标地址信息

```addr_len```: ```addr```指向的结构体的大小

上述函数与之前```TCP```用的```write```最大区别在于, 需要向它传输目标地址信息。

**接收函数**

```c
ssize_t recvfrom(int fd, void *buf, size_t n, int flags, sockaddr *addr, socklen_t *addr_len)
```

```sock```: 用于接收数据的UDP套接字文件描述符

```buff```: 保存接收数据的缓冲地址值

```n```: 可接收的最大字节数

```flags```: 一般传入```0```

```from```: 发送方地址信息

```addr_len```: ```from```指向的结构体的大小



>   好了,有了上面的基础相信很快可以看懂echo_server.c, echo_client.c 两个程序--------一个简单的回声服务器.
>
>   值得一提的是, 客户端代码中, TCP套接字通过connect函数自动分配IP地址和端口号, UDP套接字则是在sendto函数中自动分配
