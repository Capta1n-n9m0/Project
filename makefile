ARGS		 =1 2
EXECUTABLE 	=main
CC 			=gcc
CFLAGS 		=-Wall -Werror --pedantic -std=c11
LDFLAGS 	=-lm -lpthread -lcunit
SOURCES 	=$(wildcard *.c)
HEADERS 	=$(wildcard *.h)
OBJECTS 	=$(SOURCES:.c=.o)

all: $(EXECUTABLE)
	./$(EXECUTABLE) $(ARGS)

valgrind: $(EXECUTABLE)
	valgrind --leak-check=full -v ./$(EXECUTABLE) $(ARGS)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $(EXECUTABLE)

.PHONY: clean
clean:
	-rm $(EXECUTABLE) $(OBJECTS)