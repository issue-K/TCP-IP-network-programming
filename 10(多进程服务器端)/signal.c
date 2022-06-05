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
