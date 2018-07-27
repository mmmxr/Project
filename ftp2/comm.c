#include "comm.h"
#define MAX_LEN 1024
int cmd_analyse(const char* cmd)
{
    if(cmd==NULL)
    {
        return ERROR;

    }
    else if(strncmp(cmd, "ls",2)==0)   //列出本地文件列表指令
    {
        return LS;
    }
    else if(strncmp(cmd, "server ls", 9)==0)   //列出服务器文件列表指令
    {
        return SERVER_LS;
    }
    else if(strncmp(cmd, "quit", 4)==0)   //断开链接指令
    {
        return QUIT;
    }
    else if(strncmp(cmd, "download", 8)==0)    //从服务器下载文件指令
    {
        return DOWNLOAD;
    }
    else if(strncmp(cmd, "upload", 6)==0)    //上传文件到服务器指令
    {
        return UPLOAD;
    }
    else
    {
        return ERROR;
    }
}

int putlist(int sock)
{
    char buf[MAX_LEN] = {0};
    struct dirent* pd=NULL;
    DIR* pdir=opendir (".");
    while((pd = readdir(pdir))!=NULL)
    {
        if(pd->d_name[0]=='.')
        {
            continue;
        }
        sprintf(buf,"%s\n",pd->d_name);
        write(sock,buf,strlen(buf));
    }
    return 1;
}
int getlist(int sock)
{
    char buf[MAX_LEN];
    int flag = 0;

    int old_flag = fcntl(sock, F_GETFL, 0);
    int new_flag = old_flag | O_NONBLOCK;
    fcntl(sock, F_SETFL, new_flag);

    while(1)   //循环读取
    {
        int s=read(sock, buf, MAX_LEN);
        if(s<0)   //读错误
        {
            if (flag == 1){
                break;
            }
        } else if(s == 0) {
            if(flag == 1){
                break;
            }
        } else {
            flag = 1;
            printf("%s\n", buf);   //服务器端发送完毕，显示文件
            printf("\n");
        }
    }
    fcntl(sock, F_SETFL, old_flag);
    return 1;
}
int putfile(int sock,const char* filename)
{

    int fd=open(filename,O_RDONLY);
    if(fd<0)
    {
        return 0;
    }

    struct stat st;
    char header[MAX_LEN]={0};
    stat(filename,&st);

    sprintf(header,"Content-length:%lu\r\nFile_mode:%u\r\n",st.st_size,st.st_mode);
    write(sock, header,strlen(header));

    sendfile(sock,fd,NULL,st.st_size);
    return 1;
}

void Getline(int sock,char length[])
{
    char c='\0';
    int i=0;
    while(1)
    {
        read(sock,&c,1);
        if(c=='\r')
        {
            read(sock,&c,1);
            length[i]='\0';
            break;
        }
        length[i++]=c;
    }
}
int getfile(int sock, const char* filename)
{
    char length[64];
    char mode[64];
    //获取长度和权限
    Getline(sock,length);
    Getline(sock,mode);

    int len=atoi(length+15);
    int m=atoi(mode+10);
    printf("size=%d\n",len);
    printf("mode=%u\n",m);

    int fd=open(filename,O_WRONLY|O_CREAT,m);
    if(fd<0)
    {
        perror("open");
        return 0;
    }
    char c='\0';
    while(len--)
    {
        int s=read(sock,&c,1);
        if(s<0)
        {
            perror("read");
            return 0;
        }
        else if(s==0)
        {
            break;
        }
        write(fd,&c,1);
    }
    return 1;
}



