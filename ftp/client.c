#include "comm.h"
#include "login_client.h"

#define PORT 9999
#define MAX_LEN 1024


void Interface(int sock)
{
    int input;
    char answer[5];
    int target=1;
    login user;
    WelcomeMenu();
    while(target)
    {
        printf("target=%d\n",target);
        LoginMenu();
        scanf("%d",&input);
        printf("input %d\n",input);
        switch(input)
        {
        case 0://quit
            printf("Welcome to use again!\n");
            printf("*******Quit*******\n");
            close(sock);//客户端关闭文件描述符后就会自动断开连接
            exit(1);
            break;
        case 1://login
            user.flag=LOGIN;
            Login(&user);
            printf("user.logins:%d\n",user.flag);
            write(sock,&user,sizeof(user));
            read(sock,answer,sizeof(answer));//拿到服务器的结果
            printf("result=%s\n",answer);
            if(strcmp(answer,"yes")==0)
            {
                target=0;
            }
            else
            {
                printf("****登录失败!请重新登录****\n");
            }
            break;
        case 2://register
            user.flag=REGIST;
            Register(&user);
            write(sock,&user,sizeof(user));
            read(sock,answer,sizeof(answer));
            if(strcmp(answer,"yes")==0)
            {
                target=0;
            }
            else
            {
                printf("****注册失败!请重新注册****\n");
            }
            break;
        default:
            printf("********请重新选择*********\n");
            break;
        }
    }
}
void HandlerRequest(int sock)
{
    char buf[MAX_LEN];   //发送数据缓冲区
    int cmd_result;   //存放命令解析结果

    DIR* pdir;
    struct dirent *pd=NULL;
    while(1)
    {
        printf("*********Operation Help*********\n");
        printf("显示本地文件列表:             ls\n");
        printf("显示服务器文件列表:    server ls\n");
        printf("文件上传:             upload xxx\n");
        printf("文件下载:           download xxx\n");
        printf("退出:                       quit\n");
        printf("********************************\n");

        fflush(stdin);
        ssize_t s = read(0, buf, sizeof(buf));
        buf[s - 1]= '\0';
        printf("command: %s\n",buf);

        write(sock,buf,strlen(buf));//发送命令

        cmd_result = cmd_analyse(buf);   //解析输入的指令
        switch(cmd_result)
        {
        case ERROR:
            printf("ERROR!\n");
            break;
        case LS:    //列出本地文件列表
            pdir=opendir (".");
            printf("*********File List of Client*********\n");
            while((pd = readdir(pdir))!=NULL)
            {
                if(pd->d_name[0]=='.')
                {
                    continue;
                }
                printf("%s\n",pd->d_name);
            }
            printf("*********File List of Client*********\n");
            closedir(pdir);
            break;
        case SERVER_LS:   //列出服务器端
            printf("*********File List of Server*********\n");
            if(getlist(sock) == 1)
            {
                printf("*********File List of Server*********\n");
            }
            else
            {          
                printf("List file error!\n");
            }
            break;
        case DOWNLOAD:   //从服务器下载文件
            if(getfile(sock, buf+9) == 1)   //下载文件
            {
                printf("Download The File Success!\n");
            }
            else
            {
                printf("Download error!\n");
            }
            break;
        case UPLOAD:   //上传文件到服务器   
            if(putfile(sock, buf+7) == 1)//上传文件
            {
                printf("Upload The File Success!\n");
            }
            else
            {
                printf("Upload error!\n");
            }
            break; 
        case QUIT:   //断开连接
            printf("Welcome to use again!\n");
            printf("*******Quit*******\n");
            close(sock);//客户端关闭文件描述符后就会自动断开连接
            exit(1);
            break;
        default:
            break;
        }
    }
    close(sock);
    return;
}

int Init_Net(const char* ip)
{
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        perror("sock");
        exit(2);
    }

    struct sockaddr_in server;
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=inet_addr(ip);
    server.sin_port=htons(PORT);

    if(connect(sock,(struct sockaddr*)&server,sizeof(server))<0)
    {
        perror("connect");
        exit(3);
    }
    return sock;
}
int main(int argc,char* argv[])
{
    if(argc!=2)
    {
        printf("Usage:%s port\n",argv[0]);
        exit(1);
    }
    login user;
    bzero(&user, sizeof(user));

    int sock=Init_Net(argv[1]);

    Interface(sock);
    HandlerRequest(sock);

    return 0;
}
