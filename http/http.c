#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX 1024
#define HOME_PAGE "index.html"

static void Usage(const char* proc)
{
    printf("Usage:%s port\n",proc);
}
static int startup(int port)
{
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        perror("sock");
        exit(2);
    }
    int opt=1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    struct sockaddr_in local;
    local.sin_family=AF_INET;
    local.sin_addr.s_addr=htonl(INADDR_ANY);
    local.sin_port=htons(port);
    if(bind(sock,(struct sockaddr* )&local,sizeof(local))<0)
    {
        perror("socket");
        exit(3);
    }
    if(listen(sock,5)<0)
    {
        perror("listen");
        exit(4);
    }
    return sock;
}
int get_line(int sock,char line[],int size)
{
    int c='a';
    int i=0;
    ssize_t s=0;
    while(i<size&&c!='\n')
    {
        s=recv(sock,&c,1,0);
        if(s>0)
        {
            if(c=='\r')
            {
                if(recv(sock,&c,1,MSG_PEEK)>0)
                {
                    if(c!='\n')
                    {
                        c='\n';
                    }
                    else
                    {
                        recv(sock,&c,1,0);
                    }
                }
            }
            line[i++]=c;
        }
        else
        {
            break;
        }
    }
    line[i]='\0';
    return i;
}

void clear_header(int sock)
{
    //从socket中读取完剩下的数据，为了防止粘包问题
    char line[MAX];
    do{
        //printf("%s",line);
        get_line(sock,line,sizeof(line));
    }
    while(strcmp(line,"\n")!=0);

}
void echo_404(err_code)
{

}
void echo_error(int err_code)
{
    switch(err_code)
    {
        case 404:
            echo_404(err_code);
            break;
        case 501:
            break;
        default:
            break;
    }
}
void echo_www(int sock,const char* path,int size,int* err)
{
    clear_header(sock);//只从sock中读数据，为了防止粘包问题
    
    //响应
    int fd=open(path,O_RDONLY);
    if(fd<0)
    {
        *err=404;
        return;
    }
    char line[MAX];
    sprintf(line,"HTTP/1.0 200 OK\r\n");
    send(sock,line,strlen(line),0);

    sprintf(line,"\r\n");
    send(sock,line,strlen(line),0);

    sendfile(sock,fd,NULL,size);
    close(fd);
}

int exe_cgi(int sock, char path[],char method[],char* query_string)
{
    char line[MAX];
    int content_length=-1;

    char method_env[MAX/32];
    char query_string_env[MAX];

    if(strcasecmp(method,"GET")==0)
    {
        clear_header(sock);
    }
    else//POST
    {
        do{
            get_line(sock,line,sizeof(line));
            if(strncmp(line,"Content-Length: ",16)==0)
            {
                content_length=atoi(line+16);
            }
        }
        while(strcmp(line,"\n")!=0);

        if(content_length==-1)
        {
            return 404;
        }

        char query[MAX] = {0};
        read(sock, query, content_length);
        query_string = query;
    }

    sprintf(line,"HTTP/1.0 200 OK\r\n");
    send(sock,line,strlen(line),0);
    sprintf(line,"Content-Type:text/html\r\n");
    send(sock,line,strlen(line),0);
    sprintf(line,"\r\n");
    send(sock,line,strlen(line),0);

    //管道
    int input[2];
    int output[2];

    pipe(input);
    pipe(output);


    pid_t id=fork();
    if(id<0)
    {
        return 404;
    }
    else if(id==0)
    {
        close(input[1]);
        close(output[0]);

        dup2(input[0],0);
        dup2(output[1],1);


        //导出环境变量
        sprintf(method_env,"METHOD_ENV=%s",method);
        putenv(method_env);

        sprintf(query_string_env,"QUERY_STRING=%s",query_string);
        putenv(query_string_env);

        execl(path,path,NULL);
        exit(1);
    }
    else
    {
        close(input[0]);
        close(output[1]);
        char c;

        while(read(output[0],&c,1)>0)
        {
            send(sock,&c,1,0);
        }
        waitpid(id,NULL,0);

     //    if () {
     //        int fd=open("wwwroot/index.html",O_RDONLY,0);
     //        struct stat st;
     //        stat("wwwroot/index.html",&st);

     //        sendfile(sock,fd,NULL,st.st_size);
     //        perror("sendfile");
     //    }

        close(input[1]);
        close(output[0]);
    }
    return 200;
}
void* handler_request(void* arg)
{
    int sock=(int)(long)arg;
    char line[MAX];
    char method[MAX/12];
    char url[MAX];
    char path[MAX];
    int err_code=200;
    int cgi=0;
    char* query_string;
    unsigned long i=0;
    unsigned long j=0;

#if 0
    
    do{
        printf("%s",line);
        get_line(sock,line,sizeof(line));
    }
    while(strcmp(line,"\n")!=0);
        
#else
    int s = get_line(sock, line, sizeof(line));
    if(s<0)
    {
        err_code=404;//先定义为404，后改
        goto end;
    }
    
    //get method
    //请求行：GET / HTTP/1.1 

    while(i<sizeof(method)-1&&j<sizeof(line)&&!isspace(line[j]))
    {
        method[i]=line[j];
        i++;
        j++;
    }
    method[i]='\0';

    //
    if(strcasecmp(method,"GET")==0)
    {

    }
    else if(strcasecmp(method,"POST")==0)
    {
        cgi=1;
    }
    else
    {
        err_code=404;
        goto end;
    }

    while(j<sizeof(line)&&isspace(line[j]))
    {
        j++;
    }
    i=0;
    while(i<sizeof(url)-1&&j<sizeof(line)&&!isspace(line[j]))
    {
        url[i]=line[j];
        i++;
        j++;
    }
    url[i]='\0';

    if(strcasecmp(method,"GET")==0)
    {
        query_string=url;
        while(*query_string)
        {
            if(*query_string=='?')
            {
                *query_string='\0';
                query_string++;
                cgi=1;
                break;
            }
            query_string++;
        }
    }
    //method[GET,POST] cgi[0|1] url[] query_string[NULL|arg]
    //url->wwwroot/a/b/c.html
    ////url->/:首页
    sprintf(path,"wwwroot%s",url);
    if(path[strlen(path)-1]=='/')
    {
        strcat(path,HOME_PAGE);
    }

    struct stat st;
    if(stat(path,&st)<0)
    {
        err_code=404;
        goto end;
    }
    else
    {
        if(S_ISDIR(st.st_mode))
        {
            strcat(path,HOME_PAGE);
        }
        else
        {
            if((st.st_mode & S_IXUSR) ||(st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
            {
                cgi=1;
            }
        }
        if(cgi)
        {
            //exe_cgi
            err_code=exe_cgi(sock,path,method,query_string);
        }
        else
        {
            //GET
            printf("method:%s path:%s\n",method,path);
            echo_www(sock,path,st.st_size,&err_code);
        }
    }
#endif
end:
    if(err_code!=200)
    {
        echo_error(err_code);
    }
    close(sock);
    return NULL;
}
int main(int argc,char* argv[])
{
    if(argc!=2)
    {
        Usage(argv[0]);
        return 1;
    }
    signal(SIGPIPE,SIG_IGN);
    int lisen_sock=startup(atoi(argv[1]));
    for(;;)
    {
        struct sockaddr_in client;
        socklen_t len=sizeof(client);
        int new_sock=accept(lisen_sock,(struct sockaddr* )&client,&len);
        if(new_sock<0)
        {
            perror("accept");
            continue;
        }
        pthread_t id;
        pthread_create(&id,NULL,handler_request,(void*)(size_t)new_sock);
        pthread_detach(id);
    }
}
