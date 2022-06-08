### 多播

多播基于```udp```实现(用```TCP```就没意义了, 发给每个对象都需要建立连接)

#### 发送方

发送方只需要往多播地址发送```udp```多播数据包即可

为了传输多播数据包, 必须设置```TTL```(最大经过路由器数量)

```TTL```通过第九章讲到的套接字可选项完成, 与设置```TTL```相关的协议层为```IPPROTO_IP```, 选项名为```IP_MULTICAST_TTL```. 

因此, 可以用以下代码设置```TTL```为```64```

```c
int send_sock;
int time_live = 64;
...
send_sock=socket(PF_INET,SOCK_DGRAM,0);
setsockopt(send_sock,IPPROTO_IP,IP_MULTICAST_TTL,(void*)&time_live,sizeof(time_live);
...
```

#### 接收方

接收方需要加入多播组才能收到特定多播地址的消息

加入多播组也通过设置套接字可选项来完成, 相关的协议层为```IPPROTO_IP```, 选项名为```IP_ADD_MEMBERSHIP```

```c
int recv_sock;
struct ip_mreq join_adr;
...
recv_sock=socket(PF_INET,SOCK_DGRAM,0);
...
join_adr.imr_multiaddr.s_addr="多播组地址信息";
join_adr.imr_interface.s_addr="加入多播组的主机地址信息";
setsockopt(recv_sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,(void*)&join_adr,sizeof(time_live);
...
```

下面是```ip_mrep```结构体的定义

```c
struct ip_mreq
{
    struct in_addr imr_multiaddr; // 需要加入的多播组ip地址
    struct in_addr imr_interface; // 加入该组的套接字所属主机的ip地址
}
```

代码分别在```multicast_sender.c```和```multicast_receiver.c```





### 广播

广播和多播类似, 一次性向多个主机发送数据

但是多播可以跨越不同网络给特定主机传输数据(只要加入多播组), 但广播只能向同一网络的主机传输数据

比如, 希望向网络地址为```192.12.34```的所有主机传输数据时, 可以向```192.12.34.255```传输(把剩余位都置```1```)

特别的, 如果希望向本地网络广播, 使用```IP```为```255.255.255.255```

#### 实现

```receiver```端非常简单, 和普通```udp```收数据保持一致即可

```sender```端除了发送```IP```改为广播```IP```, 还需要给套接字加上可选项

```c
int send_sock;
int bcast;
...
send_sock=socket(PF_INET,SOCK_DGRAM,0);
...
setsockopt(send_sock,SOL_SOCKET,SO_BROADCAST,(void*)&bcast,sizeof(bcast));
...
```

代码分别在```broadcast_sender.c```和```broadcast_receiver.c```