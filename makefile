ARGS		=3
EXECUTABLE 	=haiku
CC 			=gcc
CFLAGS 		=-Wall -Werror --pedantic -std=c11 -g
LDFLAGS 	=-lm -lpthread -pthread -lcunit
SOURCES 	=main.c server.c client.c haiku.c queue.c
HEADERS 	=server.h client.h haiku.h queue.h
OBJECTS 	=$(SOURCES:.c=.o)

all: $(EXECUTABLE)
	./$(EXECUTABLE) $(ARGS)

standalone: standalone_build
	./server $(ARGS) & ./client $(ARGS)

standalone_build: server client

server: server.h server.c queue.h queue.c haiku.h haiku.c
	$(CC) $(CFLAGS) -DSTANDALONE server.c queue.c haiku.c -o server $(LDFLAGS)

client: client.c client.c queue.h queue.c haiku.h haiku.c
	$(CC) $(CFLAGS) -DSTANDALONE client.c queue.c haiku.c -o client $(LDFLAGS)

# to install cuint run this
# sudo apt-get install libcunit1 libcunit1-doc libcunit1-dev
run_tests: build_tests
	./tests

build_tests: tests.c server.c client.c haiku.c queue.c server.h client.h haiku.h queue.h
	$(CC) $(CFLAGS) tests.c server.c client.c queue.c haiku.c -o tests $(LDFLAGS)

helgrind: $(EXECUTABLE)
	valgrind --tool=helgrind -v ./$(EXECUTABLE) $(ARGS)

valgrind: $(EXECUTABLE)
	valgrind --leak-check=full -v ./$(EXECUTABLE) $(ARGS)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $(EXECUTABLE) $(LDFLAGS)

%.o : %.c build_headers
	$(CC) -c $(CFLAGS) $< -o $@ $(LDFLAGS)

build_headers: $(HEADERS)

.PHONY: clean
clean:
	-rm $(EXECUTABLE) $(OBJECTS) client server