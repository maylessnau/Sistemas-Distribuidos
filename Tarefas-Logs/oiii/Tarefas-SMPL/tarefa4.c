/* Autoria: Mayara Lessnau de Figueiredo Neves 
*  Data da ultima modificação: 20/05/2026
*  Finalidade: Este codigo eh uma simulacao de um Sistema Distribuido
*  com detector de falhas em anel, onde cada processo executa testes no
*  proximo do anel ate encontrar outro processo correto. Cada processo
*  tambem mantem localmento o vetor State[N]. A entrada do vetor State[j]
*  indica o estado do processo j (UNKNOWN, CORRETO ou FALHO). Quando um 
*  processo correto testa outro processo correto obtem as informacoes do 
*  estado dos demais processos do sistema.
*/

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

#define test 1
#define fault 2
#define recovery 3

#define UNKNOWN -1
#define CORRETO 0
#define FALHO 1


typedef struct {
    int id;         // identificador de facility do SMPL 
    int *State;     // vetor de estados dos processos
} TipoProcesso;

TipoProcesso *processo;

void limpeza (TipoProcesso* p, int N) {
    for (int i = 0; i < N; i++)
        free (processo[i].State);

    free (processo);
    return;
}

void imprime_state (int p, int N) {
    printf("State(%d) no tempo %4.1f:\n", p, time());

    for (int j = 0; j < N; j++)
        printf ("State[%d] = %d\n", j, processo[p].State[j]);

    printf("\n");
    return;
}

int main (int argc, char *argv[]) {

    static int N, //numero de processos do sistema distribuido
           token, //indica o processo que esta executando agora
           event, r, i, j,
           MaxTempoSimulac = 120;

    static char fa_name[5]; 
    
    if (argc != 2) {
        puts("Uso correto: ./tarefa4 <numero de processos>");
        exit(1);
    }

    N = atoi(argv[1]);

    smpl(0, "Tarefa 4 do trabalho 0 de Sistemas Distribuidos");
    reset();
    stream(1);

    // inicializar os N processos

    processo = (TipoProcesso*) malloc (sizeof(TipoProcesso)*N);

    for(i = 0; i < N; i++) {
        memset(fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name, 1);
    }

    // inicializacao do vetor State[]
    for (i = 0; i < N; i++) {
        processo[i].State = (int*) malloc (sizeof(int) * N);
        for (j = 0; j < N; j++) {
            processo[i].State[j] = UNKNOWN;
        }
        processo[i].State[i] = CORRETO; // processo dono do vetor -> correto
    }

    // mostrando vetor State[] em estado inicial
    for (j = 0; j < N; j++) {
        imprime_state(j, N);
    }

    // vamos agora fazer o escalonamento dos eventos iniciais

    for (i = 0; i < N; i++) {
        // todos os processos de 0 ate N-1 vão testar na unidade de tempo 30
        schedule(test, 30.0, i); 
    }

    //v1: todos corretos

    //v2: 1 e 4 falhos, crash 31 rec 61
    // schedule(fault, 31.0, 1);
    // schedule(recovery, 61.0, 1);
    // schedule(fault, 31.0, 4);
    // schedule(recovery, 61.0, 4);

    // v3: 1 a 6 falhos, crash 31 rec 91
    // for(int i = 1; i < 7; i++) {
    //     schedule(fault, 31.0, i);
    //     schedule(recovery, 61.0, i);
    // }

    //v4: 1 até N falhos (so 0 correto), crash 31 rec 61
    // for(int i = 1; i < N; i++) {
    //     schedule(fault, 31.0, i);
    //     schedule(recovery, 61.0, i);
    // }


    //v5: 0 até N falhos, crash 1, rec 31
    for(int i = 0; i < N; i++) {
        schedule(fault, 1.0, i);
        schedule(recovery, 31.0, i);
    } 


    // agora vem o loop principal do simulador

    while (time() < MaxTempoSimulac) {

        //pergunta se existe evento pra acontecer com o processo que tem o token
        cause(&event, &token); 
        switch(event) {
            case test:
         
                if (status(processo[token].id) != 0) break; //processo falho nao testa

                int q = (token + 1) % N; //q eh o proximo processo no anel
                int estado;

                // enquanto ainda nao percorreu todos os processos do anel
                while (q != token) {

                    estado = status(processo[q].id);

                    // achou um correto :)
                    if (estado == 0) {
                        printf ("O processo %d testou o processo %d correto no tempo %4.1f\n", token, q, time());
                        processo[token].State[q] = CORRETO;
                        i = (q + 1) % N; // i recebe o proximo do anel depois de q
                        while (i != token) { // token para quando voltar nele mesmo
                            printf ("O processo %d obtém info sobre o processo %d vinda do processo %d\n", token, i, q);
                            processo[token].State[i] = processo[q].State[i]; // atualiza com infos de q
                            i = (i + 1) % N;
                        }
                        break;
                    }

                    // se chegou aqui eh porque esta falho :(
                    printf ("O processo %d testou o processo %d falho no tempo %4.1f\n", token, q, time());
                    processo[token].State[q] = FALHO;

                    q = (q + 1) % N; // atualiza com o proximo
                }

                // se q == token entao deu uma volta no anel
                if (q == token)
                    printf ("O processo %d testou todos os processos falhos no tempo %4.1f\n", token, time());

                // mostrando vetor State[]
                imprime_state(token, N);

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
    limpeza(processo, N);
}