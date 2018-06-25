#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>

#include "./keyboard.h"

#define FC 5
#define BC 0
#define W 10
#define H 20

struct data {
	int x;
	int y;
};

struct shape {
	int s[5][5];
};

int background[H][W];

struct shape shape_arr[7] = {
	{ 0,0,0,0,0, 0,0,1,0,0, 0,1,1,1,0, 0,0,0,0,0, 0,0,0,0,0},
	{ 0,0,0,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,0,0,0},
	{ 0,0,0,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,1,0, 0,0,0,0,0},
	{ 0,0,0,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,1,1,0,0, 0,0,0,0,0},
	{ 0,0,0,0,0, 0,1,1,0,0, 0,1,1,0,0, 0,0,0,0,0, 0,0,0,0,0},
	{ 0,0,0,0,0, 0,1,1,0,0, 0,0,1,1,0, 0,0,0,0,0, 0,0,0,0,0},
	{ 0,0,0,0,0, 0,0,1,1,0, 0,1,1,0,0, 0,0,0,0,0, 0,0,0,0,0} 
};

void draw_element(int x, int y, int c)
{
	x *= 2;
	x++;
	y++;
	printf("\033[%d;%dH", y, x);
	printf("\033[3%dm\033[4%dm", c, c);
	printf("[]");
	printf("\033[?25l");
	printf("\033[0m");
	fflush(stdout);
}

void set_back(struct data *t, struct shape p)
{
	for (int i=0; i<5; i++){
		for (int j=0; j<5; j++) {
			if ( p.s[i][j] != 0 )
				background[t->y+i][t->x+j] = 1;
		}
	}
}


void draw_bk( void )
{
	for (int i=0; i<H; i++) {
		for (int j=0; j<W; j++) {
			if ( background[i][j] == 0 )
				draw_element(j, i, BC);
			else
				draw_element(j, i, FC);
		}
	}
}


void draw_shape(int x, int y, struct shape p, int c)
{
	for (int i=0; i<5; i++) {
		for (int j=0; j<5; j++) {
			if (p.s[i][j] != 0 )
				draw_element(x+j, y+i, c);
		}
	}
}

int can_move(int x, int y, struct shape p)
{
	for (int i=0; i<5; i++) {
		for (int j=0; j<5; j++) {
			if (p.s[i][j] == 0) 
				continue;
			if ( y+i >= H )
				return 0;
			if ( x+j < 0 )
				return 0;
			if ( x+j >= W )
				return 0;
			if (background[y+i][x+j] != 0 )
				return 0;
		}
	}

	return 1;
}

void clean_line( )
{
	int i;
	int total;
	for (i=0; i<H; i++) {
		total = 0;
		for (int j=0; j<W; j++) {
			if ( background[i][j] != 0 )
				total++;
		}
		if ( total == W ) {
			for (int k=i; k>0; k--) {
				memcpy(background[k], background[k-1], sizeof(background[k]));
			}
			memset(background[0], 0x00, sizeof(background[0]));
		}
	}
}


void over()
{
	int total = 0;
	for (int i=0; i<W; i++){
		if ( background[3][i] != 0 ) {
			
			printf("\033[?25h你输了\n");
			recover_keyboard();
			exit(0);
		}
	}
}

void tetris_timer(struct data *p)
{
	draw_shape(p->x, p->y, shape_arr[0], BC);
	if (can_move(p->x, p->y+1, shape_arr[0]) ) {
		p->y++;
		draw_shape(p->x, p->y, shape_arr[0], FC);
	} else {
		set_back(p, shape_arr[0]);
		draw_shape(p->x, p->y, shape_arr[0], FC);
		clean_line();
		over();
		draw_bk();
		p->y = 1;
	}
}

struct shape trun_90(struct shape p)
{
	struct shape tmp;
	for (int i=0; i<5; i++) {
		for (int j=0; j<5; j++) {
			tmp.s[i][j] =  p.s[4-j][i];
		}
	}
	return tmp;
}

void tetris(struct data *p)
{
	int n = get_key();
	if ( is_up(n) ) {
		draw_shape(p->x, p->y, shape_arr[0], BC);
		shape_arr[0] = trun_90(shape_arr[0]);
		if ( can_move(p->x, p->y, shape_arr[0]) == 0) {
			shape_arr[0] = trun_90(shape_arr[0]);
			shape_arr[0] = trun_90(shape_arr[0]);
			shape_arr[0] = trun_90(shape_arr[0]);
		}
		draw_shape(p->x, p->y, shape_arr[0], FC);
	} else if ( is_left(n) ) {
		draw_shape(p->x, p->y, shape_arr[0], BC);
		if ( can_move(p->x-1, p->y, shape_arr[0]) ) {
			p->x--;
			draw_shape(p->x, p->y, shape_arr[0], FC);
		} else {
			draw_shape(p->x, p->y, shape_arr[0], FC);
		}
	}
	else if ( is_right(n) ) {
		draw_shape(p->x, p->y, shape_arr[0], BC);
		if ( can_move(p->x+1, p->y, shape_arr[0]) ) {
			p->x++;
			draw_shape(p->x, p->y, shape_arr[0], FC);
		} else {
			draw_shape(p->x, p->y, shape_arr[0], FC);
		}
	}
	else if ( is_down(n) ) {
		draw_shape(p->x, p->y, shape_arr[0], BC);
		if ( can_move(p->x, p->y+1, shape_arr[0]) ) {
			p->y++;
			draw_shape(p->x, p->y, shape_arr[0], FC);
		} else {
			draw_shape(p->x, p->y, shape_arr[0], FC);
		}
	}
}

struct data pos = {.x=3, .y=5};

void handler(int s)
{
	tetris_timer(&pos);
}

void handler_int(int s)
{
	printf("\033[?25h");
	recover_keyboard();
	exit(0);
}

int main( void )
{
	struct sigaction act;
	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGALRM, &act, NULL);

	signal(SIGINT, handler_int);
	signal(SIGQUIT, SIG_IGN);

	struct itimerval it;
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 1;
	it.it_interval.tv_sec = 1;
	it.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &it, NULL);

	init_keyboard();
	draw_bk();
	while ( 1 ) {
		tetris(&pos);
	}

	recover_keyboard();
}

