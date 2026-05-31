/* Programa: tempo.c
 * Finalidade: aprender a programar a simulação de algoritmos distrubuídos
 * Data: 14 de maio de 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

// Vamos definir os eventos: um processo em algum instante do tempo pode sofrer um evento
#define test 1
#define fault 2
#define recovery 3

// facility deve ser traduzida como recurso
typedef struct {
    int id; //identificador de facility do SMPL
	    //outra variaveis vem aqui
} TipoProcesso;

TipoProcesso *processo;

int main (int argc, char *argv[]) {
        
	
	static int N, // númers de processos do sistema distribuído
    	token, // indica o processo que está executando
     	event, r, i,
    	MaxTempoSimulac = 120;

    static char fa_name[5]; // nome da facility

    if (argc != 2) {
        puts("Uso correto: ./tempo <numero de processos>");
        exit(1);
    }

    N = atoi(argv[1]);

    //num threads , nome pro programa
    smpl(0, "Meu primeiro programa de simulacao de sistemas distribuidos");
    reset(); // sempre que começa tem que dar reset
    stream(1);

    // inicializar os N processos
    processo = (TipoProcesso*) malloc (sizeof(TipoProcesso)*N);
 
    for(i = 0; i < N; i++) {
        memset(fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name, 1);
    }

    for (i = 0; i < N; i++) {
        schedule(test, 30.0, i); 
    }

    schedule(fault, 31.0, 1);
    schedule(recovery, 61.0, 1);

    while (time() < MaxTempoSimulac) {

        cause(&event, &token);
        switch(event) {
            case test: 
                if (status(processo[token].id) != 0) break;
                printf ("Sou o processo %d e estou testando no tempo %4.1f\n", token, time());
                schedule (test, 30.0, token);
                break;

            case fault:
                r = request (processo[token].id, token, 0);
                printf ("O processo %d falhou no tempo %4.1f\n", token, time());
                break;

            case recovery:
                release (processo[token].id, token);
                printf ("O processo %d recuperou no tempo %4.1f\n", token, time());
                schedule (test, 1.0, token);
                break;
        }
    }
    free(processo);
}

