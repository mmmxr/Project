#include <stdio.h>
#include "parse.h"
#include "extern.h"
#include "define.h"
#include "init.h"


//获取数据
char cmd_line[MAX+1];

//处理数据
char av_line[MAX+1];

command__t cmd[PIPEMAX];//用来装命令行参数


int main()
{
    sig_init();
    shell_loop();
    return 0;
}
