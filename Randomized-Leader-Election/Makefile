CC = gcc
CFLAGS = -Wall -g

BIBLIOTECAS = smpl.o rand.o
PROGRAMAS = randomized

all: randomized

randomized: randomized.o $(BIBLIOTECAS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

clean:
	rm -f *.o $(PROGRAMAS)