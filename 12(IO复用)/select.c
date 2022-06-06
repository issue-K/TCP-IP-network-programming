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
