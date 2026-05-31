CC = gcc
CFLAGS = -Wall -g

BIBLIOTECAS = smpl.o rand.o

PROGRAMAS = tarefa1 tarefa2 tarefa3 tarefa4 tempo

all: $(PROGRAMAS)

tarefa4: tarefa4.o $(BIBLIOTECAS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

tarefa3: tarefa3.o $(BIBLIOTECAS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

tarefa2: tarefa2.o $(BIBLIOTECAS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

tarefa1: tarefa1.o $(BIBLIOTECAS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

tempo: tempo.o $(BIBLIOTECAS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

clean:
	rm -f *.o $(PROGRAMAS)
