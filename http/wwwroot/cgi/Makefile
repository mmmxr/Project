.PHONY:all
all:echo_server Search_server echo_register
echo_server:echo_server.c
	gcc -o $@ $^
Search_server:Search_server.c
	gcc -o $@ $^
echo_register:echo_register.c
	gcc -o $@ $^
.PHONY:clean
clean:
	rm -f echo_server
