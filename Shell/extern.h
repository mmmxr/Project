#pragma once
#include "define.h"


extern command__t cmd[PIPEMAX];//用来装命令行参数
//获取数据
extern char cmd_line[MAX+1];

//处理数据
extern char av_line[MAX+1];

int pipenum;//用了几个管道

int lastid;//最后一个等待的子进程
