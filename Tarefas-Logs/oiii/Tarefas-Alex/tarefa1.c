/* Programa: tarefa1.c
 * Finalidade: Fazer cada um dos processos testar o seguinte no anel. Implemente o teste com a função status() do SMPL e imprimir (printf) o resultado de cada teste executado. Por exemplo: “O processo i testou o processo j correto no tempo tal.” 
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

int j = 0; // var usada para guardar o endereço do próximo

int main (int argc, char *argv[]) {
        
	
	static int N, // númers de processos do sistema distribuído
    	token, // indica o processo que está executando
     	event, i, r,
    	MaxTempoSimulac = 150;

    static char fa_name[5]; // nome da facility

    if (argc != 2) {
        puts("Uso correto: ./tempo <numero de processos>");
        exit(1);
    }

    N = atoi(argv[1]);

    //num threads , nome pro programa
    smpl(0, "Meu segundo programa de simulacao de sistemas distribuidos");
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

    for(int i = 0; i < N; i++) {
        schedule(fault, 1.0, i);
        schedule(recovery, 31.0, i);
    }

    while (time() < MaxTempoSimulac) {

        cause(&event, &token);
        switch(event) {
            case test: 
                if (status(processo[token].id) != 0) {
                    printf("Aff!! Sou o processo %d e não posso testar nada no tempo %4.1f pois estou falho!!\n", token, time());
                    break;
                }
                printf ("Oii!! Sou o processo %d e estou testando no tempo %4.1f\n", token, time());

                j = (token + 1) % N;
                if(status(processo[j].id)) // 1 eh ocupado/falho
                    printf("O processo %d testou o processo %d e ele está falho no tempo %4.1f\n", token, j, time());    
                else
                    printf("O processo %d testou o processo %d e ele está correto no tempo %4.1f\n", token, j, time());
                
                schedule (test, 30.0, token);
                break;

            case fault:
                r = request (processo[token].id, token, 0);
                printf ("Droga!! O processo %d falhou no tempo %4.1f\n", token, time());
                break;

            case recovery:
                release (processo[token].id, token);
                printf ("Viva!! O processo %d recuperou no tempo %4.1f\n", token, time());
                schedule (test, 1.0, token);
                break;
        }
        puts("------------------------------FIM DO EVENTO------------------------------\n");

    }
    free(processo);
}