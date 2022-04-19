CC 			=gcc
CFLAGS 		=-Wall -Werror --pedantic -std=c11 -g
LDFLAGS 	=-lm -lpthread -pthread -lcunit
HAIKU 		=haiku.c haiku.h

all: server client
	./client
	./server

helgrind: server client
	valgrind --tool=helgrind -v ./server
	valgrind --tool=helgrind -v ./client

valgrind: server client
	valgrind --leak-check=full -v ./server
	valgrind --leak-check=full -v ./client

server: $(HAIKU) server.c
	$(CC) $(CFLAGS) $(LDFLAGS) haiku.c server.c -o server

client: $(HAIKU) client.c
	$(CC) $(CFLAGS) $(LDFLAGS) haiku.c client.c -o client


.PHONY: clean
clean:
	-rm $(SERVER) $(CLIENT) $(OBJECTS)