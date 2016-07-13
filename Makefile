BIN=rainbow
CFLAGS=-Wall -Wextra -pedantic -std=gnu99 -O0 -g

$(BIN): main.o arg_parser.o backend.o util.o
	$(CC) $(CFLAGS) -o $(BIN) main.o arg_parser.o backend.o

main.o: main.c configuration.h arg_parser.h backend.h
arg_parser.o: arg_parser.c backend.h
backend.o: backend.c configuration.h arg_parser.h arg_parser.h
util.o: util.c util.h

clean:
	rm -f $(wildcard *.o)
	rm -f $(BIN)
