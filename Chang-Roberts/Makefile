CC = gcc
CFLAGS = -Wall -g

BIBLIOTECAS = smpl.o rand.o

all: chang-roberts

chang-roberts: chang-roberts.o $(BIBLIOTECAS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

clean:
	rm -f *.o $(PROGRAMAS)