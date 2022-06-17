```http```请求头和响应体的格式可以参照[Link](https://blog.csdn.net/Louie_0/article/details/122781892)


一个简易的```http```服务端如下所示(只处理```http```的```get```请求, 且只能返回```html```页面)

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define SMALL_BUF 100

void ERROR(char* message);
void* request_handler(void* args);
void send_error(FILE* fp);
char* content_type(char* file);
void send_data(FILE* fp,char* ct,char* file_name);

int main(int argc,char* argv[] ){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_size;
    char buf[BUF_SIZE];
    pthread_t t_id;
    serv_sock = socket( PF_INET,SOCK_STREAM,0 );
    memset( &serv_adr,0,sizeof( serv_adr ) );
    serv_adr.sin_family = AF_INET;
    char* addr = "172.19.233.18";
    //char* addr = "127.0.0.1";
    serv_adr.sin_addr.s_addr = inet_addr( addr );
    int port = 9090;
    serv_adr.sin_port = htons( port );
    if( bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr) ) == -1 ) {
        perror("bind");
        ERROR("bind() error.");
    }
    if( listen(serv_sock,20) == -1 )
        ERROR("listen() error." );
    while( 1 )
    {
        printf("begin accept\n" );
        clnt_adr_size = sizeof( clnt_adr );
        clnt_sock = accept( serv_sock,(struct sockaddr*)&clnt_adr,&clnt_adr_size);
        printf("Connection Request: %s:%d\n",
               inet_ntoa(clnt_adr.sin_addr),
               ntohs(clnt_adr.sin_port) );
        // 每个客户端开一个线程处理连接
        pthread_create( &t_id,NULL,request_handler,&clnt_sock );
        pthread_detach( t_id );
    }
    close( serv_sock );
    return 0;
}

void *request_handler(void* args){
    int clnt_sock = *((int*)args);
    char req_line[SMALL_BUF];
    FILE* clnt_read;
    FILE* clnt_write;
    char method[10];
    char ct[15];
    char file_name[30];

    clnt_read = fdopen( clnt_sock,"r" );
    clnt_write = fdopen( dup(clnt_sock),"w" );
    // 第一行数据形如 GET /index.html HTTP/1.1
    fgets( req_line,SMALL_BUF,clnt_read );
    if(  strstr(req_line,"HTTP/") == NULL )
    {
        send_error( clnt_write ); // 不是合法的http请求, 报错
        fclose( clnt_read );
        fclose( clnt_write );
        return NULL;
    }
    strcpy( method,strtok(req_line," /") );
    strcpy( file_name,strtok(NULL," /") );
    strcpy( ct,content_type(file_name) );
    if( strcmp(method,"GET") != 0 )  // 目前只支持get
    {
        send_error( clnt_write );
        fclose( clnt_read );
        fclose( clnt_write );
        return NULL;
    }
    fclose( clnt_read );
    send_data( clnt_write,ct,file_name );  //响应数据
}
void send_data(FILE* fp,char* ct,char* file_name){
    char protocol[] = "HTTP/1.0 200 OK\r\n";
    char server[] = "Server:Linux Web Server \r\n";
    char cnt_len[] = "Content-length:2048\r\n";
    char cnt_type[SMALL_BUF];

    sprintf( cnt_type,"Content-type:%s\r\n\r\n",ct );
    FILE* send_file = fopen(file_name,"r" );
    if( send_file == NULL ){
        send_error( fp );
        return;
    }
    fputs( protocol,fp );
    fputs(server,fp );
    fputs(cnt_len,fp);
    fputs(cnt_type,fp);
    char buf[BUF_SIZE];
    while( fgets(buf,BUF_SIZE,send_file) != NULL ){
        fputs( buf,fp );
        fflush( fp );
    }
    fflush( fp );
    fclose( fp );
}
char* content_type(char* file){
    char extendsion[SMALL_BUF];
    char file_name[SMALL_BUF];
    strcpy( file_name,file ); // copy一份
    strtok( file_name,"." );
    strcpy( extendsion,strtok(NULL,".") );// 取得扩展名
    if( !strcmp(extendsion,"html") || !strcmp(extendsion,"htm") )
        return "text/html";
    else
        return "text/plain";
}
void send_error(FILE* fp){
    char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
    char server[] = "Server:Linux Web Server \r\n";
    char cnt_len[] = "Content-length:2048\r\n";
    char cnt_type[] = "Content-type:text/html\r\n\r\n";
    char content[] = "<html><head><title>NETWORK</title></head>"
                     "<body><font size=+5><br>发生错误！ 查看请求文件名和请求方式!"
                     "</font></body></html>";
    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_len, fp);
    fputs(cnt_type, fp);
    fflush(fp);
}

void ERROR(char* message){
    printf("[ ERROR ]: %s\n",message );
    exit( 1 );
}
```
