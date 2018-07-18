#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct Creat_memo{
    char year[5];
    char month[4];
    char day[4] ;
    char route[128] ;
}c_memo;
int main()
{
    char query[1024] ={0};
    c_memo memo;
    sprintf(query, "%s", getenv("QUERY_STRING"));

    //date=2018-07-11&route=dddd
    strcpy(query,query+5);
    //2018-07-11&route=dddd

    //2018
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

    //route
    strcpy(memo.route,query);

    printf("year:%s,month:%s,day:%s\n",memo.year,memo.month,memo.day);
    printf("route:%s\n",memo.route);
    
    // TODO a. query 放到数据库
    //
    // 跳转至主页

}
