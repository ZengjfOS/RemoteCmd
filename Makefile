all: server client

server: server.o cs_socket.o talk.o
	gcc server.o cs_socket.o talk.o -o server

client: client.o cs_socket.o talk.o
	gcc client.o cs_socket.o talk.o -o client

server.o: src/server.c
	gcc -c src/server.c

client.o: src/client.c
	gcc -c src/client.c

cs_socket.o: src/cs_socket.c src/cs_socket.h
	gcc -c src/cs_socket.c

talk.o: src/talk.c src/talk.h
	gcc -c src/talk.c

clean:
	rm -rf *.o server client
