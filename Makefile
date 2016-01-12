BIN=rainbow
CC=gcc
FLAGS=-Wall -Wextra -pedantic -std=gnu99 -O0 -g

$(BIN): main.o arg_parser.o backend.o
	$(CC) $(FLAGS) -o $(BIN) main.o arg_parser.o backend.o

main.o: main.c
	$(CC) $(FLAGS) -c -o main.o main.c

arg_parser.o: arg_parser.c
	$(CC) $(FLAGS) -c -o arg_parser.o  arg_parser.c

backend.o: backend.c
	$(CC) $(FLAGS) -c -o backend.o backend.c

clean:
	rm *.o
	rm $(BIN)
