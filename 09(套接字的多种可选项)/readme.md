这章就不写了, 挂个别人的详解[LINK](https://github.com/riba2534/TCP-IP-NetworkNote/tree/master/ch09)

主要需要关注的```socket```选项有

```c
SO_SNDBUF   // 输出缓冲大小
SO_RCVBUF   // 输入缓冲大小
SO_REUSEADDR  // 值为0意味着处于time_wait状态下的套接字端口号不可使用, 为1可以使用
TCP_NODELAY  // 是否启用nagle算法

```
