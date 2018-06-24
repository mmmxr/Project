#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
typedef struct node_{
    char* name;
    struct node_ *next;
    struct node_ *prev;
}node_t;

typedef enum{
    STOP,
    PLAY,
    PAUSE
}STATUS;

node_t* head=NULL;
node_t* cur=NULL;

int first=1;
int play=STOP;

void list_insert(char* name)
{
    node_t *p=(node_t *)malloc(sizeof(node_t));
    memset(p,0x00,sizeof(node_t ));
    p->name=(char*)malloc(strlen(name)+1);
    strcpy(p->name,name);
    p->next=NULL;
    p->prev=NULL;
    if(head==NULL)
    {
        p->next=p;
        p->prev=p;
        head=p;
        cur=head;
    }
    else
    {
        p->prev=head->prev;
        p->next=head;
        head->prev->next=p;
        head->prev=p;
    }
}
void list_show()
{
    node_t* c=head;
    if(c==NULL)
    {
        return;
    }
    while(c!=head)
    {
        printf("%s",c->name);
        
        if(cur==c)
        {
            printf("<======当前正在播放");
        }
        printf("\n");
        c=c->next;
    }
}
void music_read()
{
    DIR* pdir=opendir("./music");
    if(pdir==NULL)
    {
        printf("没有\n");
        exit(1);
    }
    struct dirent* pd=NULL;
    char buf[1024];
    while((pd=readdir(pdir))!=NULL)
    {
        if(pd->d_name[0]=='.')
        {
            continue;
        }
        sprintf(buf,"./music/%s",pd->d_name);
        printf("%s\n",buf);
        //list_insert(buf);
        memset(buf,0x00,sizeof(buf));
    }
    closedir(pdir);
}
int menu()
{
    printf("===========我的播放器=========\n");
    list_show();
    printf("===========请选择=============\n");
    printf("=========1.播放/暂停==========\n");
    printf("=========2.下一首=============\n");
    printf("=========3.上一首=============\n");
    printf("=========4.停止===============\n");
    printf("=========0.退出===============\n");
    printf(">");
    int choose=1;
    scanf("%d",&choose);
    return choose;
}
void playpause()
{
    if(first==1)
    {
        printf("hehe\n");
        char buf[1024];
        sprintf(buf,"madplay -o wav:- %s 2>/dev/null | aplay > /dev/null 2>&1 &",cur->name);
        system(buf);
        first=0;
        play=PLAY;
    }
    else
    {
        if(play==PLAY)
        {
            system("killall -SIGSTOP aplay");
            play=PAUSE;
        }
        else if(play==PAUSE)
        {
            system("killall -SIGCONT aplay");
            play=PLAY;
        }
    }
}
void stopmusic()
{
    system("killall -SIGKILL aplay");
    play=STOP;
    first=1;
}
void prevmusic()
{
    cur=cur->prev;
    stopmusic();
    playpause();
}
void nextmusic()
{
    cur=cur->next;
    stopmusic();
    playpause();
}
int main()
{
    int choose=1;
    music_read();
    while(choose!=0)
    {
        choose=menu();
        switch(choose)
        {
        case 1:
            playpause();
            break;
        case 2:
            nextmusic();
            break;
        case 3:
            prevmusic();
            break;
        case 4:
            stopmusic();
            break;
        case 0:
            system("killall -SIGKILL aplay");
            break;
        default:
            break;
        }
    }
}
