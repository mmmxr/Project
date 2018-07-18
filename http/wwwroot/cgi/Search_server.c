#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct Search_memo{
    char year[5];
    char month[4];
    char day[4] ;
}s_memo;
int main()
{
    char query[1024] ={0};
    s_memo memo;
    sprintf(query, "%s", getenv("QUERY_STRING"));

    strcpy(query, query+7);

    strncpy(memo.year,query,4);
    memo.year[4]= '\0';
    strcpy(query,query+5);

    //07
    strncpy(memo.month,query,2);
    memo.month[2]= '\0';
    strcpy(query,query+3);

    //05
    strncpy(memo.day,query,2);
    memo.day[2]= '\0';
    strcpy(query,query+9);

    printf("year:%s,month:%s,day:%s\n",memo.year,memo.month,memo.day);

    // 参数拿到了
    // TODO 
    //  a. query 放到数据库中查询
    //  b. 返回查询结果的html

}
