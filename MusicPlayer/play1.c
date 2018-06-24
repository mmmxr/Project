#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

typedef struct  node_ {
	char *name;
	struct node_ *prev;
	struct node_ *next;
}node_t;
typedef enum {STOP, PLAY, PAUSE}Status;

node_t *head = NULL; // 歌曲链表
node_t *cur  = NULL; // 指向当前播放的歌曲
int first = 1; // 是否是第一次
int play = STOP;

void list_insert(char *name)
{
	node_t *p = (node_t*)malloc(sizeof(node_t));
	memset(p, 0x00, sizeof(node_t));

	p->name =(char*)malloc(strlen(name)+1);
	strcpy(p->name, name);
	p->next = NULL;
	p->prev = NULL;

	if ( head == NULL ) {
		p->next = p;
		p->prev = p;
		head = p;
		cur = head;
	} else {
		p->prev = head->prev;
		p->next = head;
		head->prev->next = p;
		head->prev = p;
	}
}

void list_show( void )
{
	node_t *c = head;
	
	if ( c == NULL )
		return;

	do {
		printf("%s", c->name);
		if ( cur == c )
			printf(" <=== cur");
		printf("\n");
		
		c = c->next;
	} while ( c != head );
}

void mus_read( void )
{
	DIR *pdir = opendir("./music");
	if ( pdir == NULL) {
			printf("没有音乐，好好写代码\n");
			exit(1);
	}

	struct dirent *pd = NULL;
	char buf[1024] = {};
	while ( (pd=readdir(pdir)) != NULL ) {
		if ( pd->d_name[0] == '.' ) continue;
		sprintf(buf, "./music/%s", pd->d_name);
		list_insert(buf);
		memset(buf, 0x00, sizeof buf);
	}

	closedir(pdir);
}

int menu( void )
{
	printf("=============我的播放器==========\n");
	list_show();
	printf("---------------------------------\n");
	printf("\t1. 播放/暂停\n");
	printf("\t2. 下一首\n");
	printf("\t3. 上一首\n");
	printf("\t4. 停止\n");
	printf("\t0. 退出\n");
	printf("> ");
	int choose;
	scanf("%d", &choose);

	return choose;
}

void playPause()
{
	if ( first == 1 ) {
		char buf[1024] = {};
		sprintf(buf, "madplay -o wav:- %s 2>/dev/null | aplay > /dev/null 2>&1 &", cur->name);
		system(buf);
		first = 0;
		play = PLAY;
	} else {
		if ( play == PLAY ) {
			system("killall -SIGSTOP aplay");
			play = PAUSE;
		} else if ( play == PAUSE ) {
			system("killall -SIGCONT aplay");
			play = PLAY;
		}
	}
}

void stop()
{
	system("killall -SIGKILL aplay");
	play = STOP;
	first = 1;
}

void next()
{
	cur = cur->next;
	stop();
	playPause();
}

void prev()
{
	cur = cur->prev;
	stop();
	playPause();
}

int main( void )
{
	int choose = 0;

	mus_read();

	do {
		choose = menu();
		switch ( choose ) {
		case 1:
			playPause();
		break;
		case 2:
			next();
		break;
		case 3:
			prev();
		break;
		case 4:
			stop();
		break;
		case 0:
			system("killall -SIGKILL aplay");
		break;
		}
	} while (choose != 0 );
}


