#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "parse.h"
#include "extern.h"
#include "init.h"
#include "define.h"

static void cmd_print()
{
    int i=0;
    for(i=0;i<=pipenum;++i)
    {
        printf("i=%d\n",i);
        int j=0;
        for(j=0;cmd[i].argv[j]!=NULL;++j)
        {
            printf("\t[%s]\n",cmd[i].argv[j]);
        }
    }
}
void shell_loop()
{
    while(1)
    {
        printf("[myshell]#");
        init();
        if(cmd_read()==0)
        {
            break;
        }
        cmd_parse();
        cmd_print();
        cmd_execute();
    }
    printf("\n");
}
//读取命令行参数
//返回值：成功 1
int cmd_read()
{
    return (fgets(cmd_line,MAX,stdin)==NULL) ? 0:1;
}
//解析命令行参数
int cmd_parse()
{
    char* p=cmd_line;
    char* q=av_line;

    int i=0;
    for(i=0;i<PIPEMAX && *p!='\0';++i)
    {
        int j=0;
        for(j=0;j<ARGS && *p!='\0';++j)
        {
            while(*p!='\0'&& (*p==' '||*p=='\t'||*p=='\n'))
            {
                p++;
            }
            cmd[i].argv[j]=q;
            while(*p!='\0'&&*p!=' '&&*p!='\t'&&*p!='\n'&&*p!='|')
            {
                *q++=*p++;
            }
            *q++='\0';
            if(*p=='|')
            {
                pipenum++;
                p++;
                cmd[i].argv[j]=NULL;
                break;
            }
            if(*p=='\n')
            {
                cmd[i].argv[j+1]=NULL;
                break;
            }
        }
    }
    return 0;
}
//执行命令行参数
int cmd_execute()
{
    int i=0;
    int fds[2];
    for(i=0;i<=pipenum;++i)
    {
        if(i<pipenum)
        {
            pipe(fds);
            cmd[i].outfd=fds[1];
            cmd[i].infd=fds[0];
        }
        pid_t id=fork();
        if(id<0)
        {
            perror("fork");
        }
        else if(id==0)
        {
            if(cmd[i].infd!=0)
            {
                close(0);
                dup(cmd[i].infd);
            }
            if(cmd[i].outfd!=1)
            {
                close(1);
                dup(cmd[i].outfd);
            }
            int ret=execvp(cmd[i].argv[0],cmd[i].argv);
            if(ret==-1)
            {
                perror("execvp");
            }
            exit(1);
        }
        else
        {
            if(cmd[i].infd!=0)
            {
                close(cmd[i].infd);
            }
            if(cmd[i].outfd!=1)
            {
                close(cmd[i].outfd);
            }
            lastid=id;
        }
    }
    while(wait(NULL)!=lastid)
        ;
    
    return 0;
}
