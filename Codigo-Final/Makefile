CC = gcc
CFLAGS = -Wall -g

BIBLIOTECAS = smpl.o rand.o fila.o randomized.o chang-roberts.o
PROGRAMAS = main

all: main

main: main.o $(BIBLIOTECAS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

clean:
	rm -f *.o $(PROGRAMAS)