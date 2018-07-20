#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#define LS          1   //列出客户端所有文件
#define SERVER_LS   2   //列出服务器所有文件
#define DOWNLOAD    3   //下载文件
#define UPLOAD      4   //上传文件
#define QUIT        5   //退出
#define ERROR       -1   //错误


int cmd_analyse(const char* cmd);
int getlist(int sock);
int putlist(int sock);
int getfile(int sock,const char* filename);
int putfile(int sock,const char* filename);
