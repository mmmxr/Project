#include "login_server.h"
#define MAX_LEN 1024

char sql[MAX_LEN];
MYSQL m_conn;

int InsertSql(login* user)
{
    mysql_init(&m_conn);
    if(!mysql_real_connect(&m_conn,"localhost","root","root","File",3306,NULL,0))
    {
        perror("mysql_real_connect");
        return -1;//链接失败
    }
    sprintf(sql,"insert into File_Server values('%s','%s');",user->name,user->password);
    if(mysql_query(&m_conn,sql))
    {
        perror("mysql_real_connect");
        return -1;//链接失败
    }
    MYSQL_RES *result = mysql_store_result(&m_conn);
    //释放结果集
    mysql_close(&m_conn);
    return 0;

}
int SelectSql(login* user)
{
    mysql_init(&m_conn);
    if(!mysql_real_connect(&m_conn,"localhost","root","root","File",3306,NULL,0))
    {
        perror("mysql_real_connect");
        return -1;//链接失败
    }
    sprintf(sql,"select * from File_Server where name='%s';",user->name);
    if(mysql_query(&m_conn,sql))
    {
        perror("mysql_real_connect");
        return -1;//链接失败
    }
    MYSQL_RES *result = mysql_store_result(&m_conn);
    //获取行数
    my_ulonglong rows = mysql_num_rows(result);
    //获取列数                                                                                                                                    
    unsigned cols = mysql_num_fields(result);
    //获取列名
    MYSQL_FIELD *field = mysql_fetch_fields(result);
    unsigned int i = 0;
    for(; i < cols; ++i)
    {   
        printf("列名:%s\n",field[i].name);
    }   
    //获取列表内容
    MYSQL_ROW line;
    for(i = 0; i < rows ; ++i)
    {   
        line =  mysql_fetch_row(result);
        unsigned int j = 1;
        printf("该用户的密码：%s\n",line[j]);
        if(strcmp(line[j],user->password)==0)
        {
            printf("匹配结果是：%d\n",strcmp(line[j],user->password));
            return 1;
        }
        printf("\n");
    }   
    //释放结果集
    mysql_close(&m_conn);
    return 0;
}
