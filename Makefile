.PHONY: all clean run

CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: parent child

parent: parent.c
	$(CC) $(CFLAGS) -o parent parent.c

child: child.c
	$(CC) $(CFLAGS) -o child child.c

clean:
	rm -f parent child

run: all
	./parent

