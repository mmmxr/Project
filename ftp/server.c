#include "comm.h"
#include "login_server.h"

#define PORT 9999
#define MAX_LEN 1024

int Startup(int port)
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
int Check(int sock)
{
    login user;
    char answer[5]={0};
    while(1)
    {
        int s=read(sock,&user,sizeof(user));
        printf("id %s passwd %s\n", user.name, user.password);
        if(s<=0)
        {
            perror("read");
        }
        if(user.flag==LOGIN)
        {
            if(SelectSql(&user))
            {
                strcpy(answer,"yes");
                write(sock,answer,strlen(answer));
                return 1;
            }
            else
            {
                strcpy(answer,"no");
                write(sock,answer,strlen(answer));
            }
        }
        else if(user.flag==REGIST)
        {
            if(InsertSql(&user))
            {
                printf("user:%d\n",user.flag);
                strcpy(answer,"yes");
                write(sock,answer,strlen(answer));
                return 1;
            }
            else
            {
                printf("user:%d\n",user.flag);
                strcpy(answer,"no");
                write(sock,answer,strlen(answer));
            }
        }
        else
        {
            close(sock);
            return -1;
        }
    }
}
void* handler_request(void* arg)
{
    char recv_buf[MAX_LEN] = {0};   
    int cmd_result = 0;
    int sock=(int)(long)arg; 
    while(1)
    {
        printf("Client connect success!\n");

        //////判断是登录还是注册///////////
        int ret=Check(sock);
        printf("check=%d\n",ret);
        ///////////////////////////////////
        while(1)
        {
            int s=read(sock,recv_buf, sizeof(recv_buf));   //接收命令
            if(s<=0)
            {
                perror("read\n");
                break;
            }
            else
            {
                recv_buf[s] = '\0';
                cmd_result = cmd_analyse(recv_buf);   //解析命令

                switch(cmd_result)
                {
                case ERROR:
                    printf("ERROR!\n");
                    break;
                case SERVER_LS:  //查看服务器文件列表
                    if(putlist(sock) == 1)
                    {
                        printf("Put list success!\n");
                    }
                    else
                    {
                        printf("Put list error!\n");
                    }
                    break;
                case DOWNLOAD:   //客户端从服务器下载文件
                    if(putfile(sock, recv_buf+9) == 1)
                    {
                        printf("Put files success!\n");
                    }
                    else
                    {
                        printf("Put files error!\n");
                    }
                    break;
                case UPLOAD:   //客户端上传文件到服务器
                    if(getfile(sock, recv_buf+7) == 1)
                    {
                        printf("Get files success!\n");
                    }
                    else
                    {
                        printf("Get files error!\n");
                    }
                    break;
                default:
                    break;
                }
            }
        }
        close(sock);
        break;
    }
    pthread_exit(arg);
}
int main()
{
    int sock=Startup(PORT);
    while(1)
    {
        struct sockaddr_in client;
        socklen_t len=sizeof(client);
        int new_sock=accept(sock,(struct sockaddr* )&client,&len);
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
