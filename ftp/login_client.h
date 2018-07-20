#pragma once
#include <stdio.h>
#include "string.h"
#include "/usr/include/mysql/mysql.h"

typedef enum Flag
{
    LOGIN,
    REGIST,
    EXIT
}Flag;
typedef struct Login
{
    char name[12];
    char password[6];
    Flag flag;
}login;



void WelcomeMenu();
void LoginMenu();
void Login(login* l);
void Register(login* user);
