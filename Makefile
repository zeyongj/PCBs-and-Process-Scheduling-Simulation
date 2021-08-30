CC=gcc
CFLAGS=-std=c99
DBFLAGS=-g

objects=main.o list.o process.o

all: $(objects)
	$(CC) $(CFLAGS) -o main $(objects)

main.o: main.c list.h process.h
	$(CC) -c $(CFLAGS) main.c

process.o: process.h list.h
	$(CC) -c $(CFLAGS) process.c

clean:
	rm -rf main main.o process.o