.PHONY:clean shell
OBJS = shell.o parse.o init.o
CC=gcc
CFLAGS=-Wall -g
shell : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)
%.o:%.c
	$(CC) -c $< -o $@
clean:
	rm -f $(OBJS)
