OBJECTS = \
	src/main.o \
	src/game.o \
	src/board.o \
	src/ships.o

CFLAGS = -g -Wall -Wextra -std=c99 -finput-charset=UTF-8 -fexec-charset=UTF-8

all: program

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

program: $(OBJECTS)
	gcc -o program $(OBJECTS)

clean:
	rm -f $(OBJECTS) program
