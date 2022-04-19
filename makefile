ARGS		=2
EXECUTABLE 	=main
CC 			=gcc
CFLAGS 		=-Wall -Werror --pedantic -std=c11 -g
LDFLAGS 	=-lm -lpthread -pthread
SOURCES 	=$(wildcard *.c)
HEADERS 	=$(wildcard *.h)
OBJECTS 	=$(SOURCES:.c=.o)

all: $(EXECUTABLE)
	./$(EXECUTABLE) $(ARGS)

standalone: server client
	./server & ./client

server: server.h server.c queue.h queue.c haiku.h haiku.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DSTANDALONE server.c queue.c haiku.c -o server

client: client.c client.c queue.h queue.c haiku.h haiku.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DSTANDALONE client.c queue.c haiku.c -o client

helgrind: $(EXECUTABLE)
	valgrind --tool=helgrind -v ./$(EXECUTABLE) $(ARGS)

valgrind: $(EXECUTABLE)
	valgrind --leak-check=full -v ./$(EXECUTABLE) $(ARGS)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $(EXECUTABLE)

.PHONY: clean
clean:
	-rm $(EXECUTABLE) $(OBJECTS) client server