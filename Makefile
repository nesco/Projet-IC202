.PHONY: clean all

CFLAGS=-pedantic -Wall --std=gnu11 -g
LDFLAGS=-lpthread

all: server client

chatroom.o: chatroom.c chatroom.h common.h

common.o: common.c common.h
server.o: server.c chatroom.h common.h
client.o: client.c common.h

server: server.o chatroom.o common.o
client: client.o common.o 

clean:
	@rm -rf *.o *~
	@rm -rf server client

realclean: clean
