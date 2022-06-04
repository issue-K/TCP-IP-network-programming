### 利用域名获得IP地址

```c
 hostent *gethostbyname(const char *name)
```

只要传递域名字符串即返回对应的IP地址, 失败返回```NULL```指针

其中```hostent```结构体定义如下

```c
struct hostent
{
  char *h_name;			/* Official name of host.  */
  char **h_aliases;		/* Alias list.  */
  int h_addrtype;		/* Host address type.  */
  int h_length;			/* Length of address.  */
  char **h_addr_list;		/* List of addresses from name server.  */
};
```

域名转换时只需要关注```h_addr_list```即可

由于域名可以对应着多个IP, 所以```h_addr_list```是一个数组

然而该数组的每一项并不像表面上是一个```char*```类型的字符串, 而是一个```in_addr```结构体变量的指针

所以想获取IP还需要通过以下函数转化为字符串

```c
char *inet_ntoa (struct in_addr __in)
```

>题外话,各位是否觉得```hostent```结构体中直接声明为```inaddr* h_addr_list```更清晰?
>
>确实如此, 然而IP并不只有IPV4, 这样声明不具有通用性.
>
>既然IP不确定类型, 不妨声明为void*, 不过这些函数的定义在void指针标准化之前, 所以就用char了.

利用域名获取IP的程序详见```gethostbyname.c```
