#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "define.h"
#include "init.h"
#include "extern.h"

void handler(int s)
{
    (void)s;
    printf("\n[myshell]# ");
    fflush(stdout);
}
void sig_init()
{
    signal(SIGINT,handler);
    signal(SIGQUIT,SIG_IGN);
}

//初始化全局变量
void init()
{
    memset(cmd_line,0x00,sizeof(cmd_line));
    memset(av_line,0x00,sizeof(av_line));
    int i=0;
    int k=0;
    for(k=0;k<PIPEMAX;k++)
    {
        for(i=0;i<ARGS;i++)
        {
            cmd[k].argv[i]=NULL;
        }
        cmd[k].infd=0;
        cmd[k].outfd=1;
    }
    pipenum=0;
}

