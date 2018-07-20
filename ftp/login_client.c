#include "login_client.h"
#define MAX_LEN 1024
char sql[MAX_LEN];

void WelcomeMenu()
{
    printf("\n");
    printf("************************************\n");
    printf("*****                          *****\n");
    printf("*****   Welcome File Storage    ****\n");
    printf("*****                          *****\n");
    printf("*****      Have Good Time!     *****\n");
    printf("*****                          *****\n");
    printf("************************************\n");
    printf("************************************\n");
    printf("\n");

}

void LoginMenu() 
{
    printf("\n");
    printf("************************************\n");
    printf("***********    请选择    ***********\n");
    printf("************************************\n");
    printf("***********   1.登录     ***********\n");
    printf("***********              ***********\n");
    printf("***********   2.注册     ***********\n");
    printf("***********              ***********\n");
    printf("***********   0.退出登录 ***********\n");
    printf("************************************\n");
    printf("\n");
    printf("Please Enter Your Choose##  ");
}

void Login(login* user)
{
    printf("************************************\n");
    printf("********Welcome Use Again!**********\n");
    printf("************************************\n");
    printf("**i*******Username >  **************\n");
    scanf("%s",user->name);
    printf("*****Password >  *********\n");
    scanf("%s",user->password);
    printf("*************************\n");
}


void Register(login* user)
{
    printf("**************************\n");
    printf("**************************\n");
    printf("*****Hello New User!******\n");
    printf("**Please input your info**\n");
    printf("**************************\n");
    printf("*****Username >  *********\n");
    scanf("%s",user->name);
    printf("*****Password >  *********\n");
    scanf("%s",user->password);
    printf("*************************\n");
}
