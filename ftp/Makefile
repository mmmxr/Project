.PHONY:all
all:server client
server:server.c comm.c login_server.c -lpthread
	gcc -g -o $@ $^ -I/usr/include/mysql -L/usr/lib/mysql  -L/usr/lib64/mysql -lmysqlclient
client:client.c comm.c login_client.c
	gcc -o $@ $^ 
.PHONY:clean
clean:
	rm -f server client
