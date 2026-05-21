/* Autoria: Mayara Lessnau de Figueiredo Neves 
*  Data da ultima modificacao: 12/05/2026
*  Finalidade: Este codigo eh uma simulacao de um Sistema Distribuido
*  com detector de falhas em anel, onde cada processo executa testes no
*  proximo do anel.
*/

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

#define test 1
#define fault 2
#define recovery 3

typedef struct {
    int id;     //identificador de facility do SMPL 
} TipoProcesso;

TipoProcesso *processo;

int main (int argc, char *argv[]) {

    static int N, //numero de processos do sistema distribuido
           token, //indica o processo que esta executando agora
           event, r, i,
           MaxTempoSimulac = 120;

    static char fa_name[5]; 

    if (argc != 2) {
        puts("Uso correto: ./tarefa1 <numero de processos>");
        exit(1);
    }

    N = atoi(argv[1]);

    smpl(0, "Tarefa 1 do trabalho 0 de Sistemas Distribuidos");
    reset();
    stream(1);

    // inicializar os N processos

    processo = (TipoProcesso*) malloc (sizeof(TipoProcesso)*N);

    for(i = 0; i < N; i++) {
        memset(fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name, 1);
    }

    // Vamos agora fazer o escalonamento dos eventos iniciais

    for (i = 0; i < N; i++) {
        // todos os processos de 0 ate N-1 vao testar na unidade de tempo 30
        schedule(test, 30.0, i); 
    }

    schedule(fault, 31.0, 1);
    schedule(recovery, 61.0, 1);

    // agora vem o loop principal do simulador

    while (time() < MaxTempoSimulac) {

        cause(&event, &token);
        switch(event) {
            case test: 
                if (status(processo[token].id) != 0) break; //processo falho nao testa
                int j = (token + 1) % N; //j eh o proximo processo no anel
                int estado = status(processo[j].id);
                if (estado == 0) {
                    printf ("O processo %d testou o processo %d correto no tempo %4.1f\n", token, j, time());
                } else {
                    printf ("O processo %d testou o processo %d falho no tempo %4.1f\n", token, j, time());
                }
                
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