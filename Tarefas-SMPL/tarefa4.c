/* Autoria: Mayara Lessnau de Figueiredo Neves 
*  Data da última modificação: 20/05/2026
*  Finalidade: Este código é uma simulação de um Sistema Distribuído
*  com detector de falhas em anel, onde cada processo executa testes no
*  próximo do anel até encontrar outro processo correto. Cada processo
*  também mantém localmento o vetor State[N]. A entrada do vetor State[j]
*  indica o estado do processo j (UNKNOWN, CORRETO ou FALHO). Quando um 
*  processo correto testa outro processo correto obtém as informações do 
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
    int id;     //identificador de facility do SMPL 
} TipoProcesso;

TipoProcesso *processo;

void imprime_state (int p, int N, int state[]) {

    printf("State(%d) no tempo %4.1f:\n", p, time());
    for (int j = 0; j < N; j++) {
        printf ("State(%d)[%d] = %d\n", p, j, state[j]);
    }
    printf("\n");

    return;
}

int main (int argc, char *argv[]) {

    static int N, //número de processos do sistema distribuído
           token, //indica o processo que está executando agora
           event, r, i, j,
           MaxTempoSimulac = 120;

    static char fa_name[5]; 
    
    if (argc != 2) {
        puts("Uso correto: ./tarefa3 <numero de processos>");
        exit(1);
    }

    N = atoi(argv[1]);
    int State[N][N];  //vetor de estados 

    smpl(0, "Tarefa 3 do trabalho 0 de Sistemas Distribuidos");
    reset();
    stream(1);

    // inicializar os N processos

    processo = (TipoProcesso*) malloc (sizeof(TipoProcesso)*N);

    for(i = 0; i < N; i++) {
        memset(fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name, 1);
    }

    // vamos agora fazer o escalonamento dos eventos iniciais

    for (i = 0; i < N; i++) {
        // todos os processos de 0 até N-1 vão testar na unidade de tempo 30
        schedule(test, 30.0, i); 
    }

    // inicialização do vetor State[]
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            State[i][j] = UNKNOWN;
        }
        State[i][i] = CORRETO; // processo dono do vetor -> correto
    }

    schedule(fault, 31.0, 1);
    schedule(recovery, 61.0, 1);

    // mostrando vetor State[] em estado inicial
    for (j = 0; j < N; j++) {
        imprime_state(j, N, State[j]);
    }

    // agora vem o loop principal do simulador

    while (time() < MaxTempoSimulac) {

        //pergunta se existe evento pra acontecer com o processo que tem o token
        cause(&event, &token); 
        switch(event) {
            case test:
         
                if (status(processo[token].id) != 0) break; //processo falho não testa

                int q = (token + 1) % N; //q é o póximo processo no anel
                int estado;

                // enquanto ainda não percorreu todos os processos do anel
                while (q != token) {

                    estado = status(processo[q].id);

                    // achou um correto :)
                    if (estado == 0) {
                        printf ("O processo %d testou o processo %d correto no tempo %4.1f\n", token, q, time());
                        State[token][q] = CORRETO;
                        i = (q + 1) % N; // i recebe o próximo do anel depois de q
                        while (i != token) { // para quando voltar nele mesmo
                            printf ("O processo %d obtém info sobre o processo %d vinda do processo %d\n", token, i, q);
                            State[token][i] = State[q][i]; // atualiza com infos de q
                            i = (i + 1) % N;
                        }
                        break;
                    }

                    // se chegou aqui é porque está falho :(
                    printf ("O processo %d testou o processo %d falho no tempo %4.1f\n", token, q, time());
                    State[token][q] = FALHO;

                    q = (q + 1) % N; // atualiza com o próximo
                }

                // se q == token então deu uma volta no anel
                if (q == token)
                    printf ("O processo %d testou todos os processos falhos no tempo %4.1f\n", token, time());

                // mostrando vetor State[]
                imprime_state(token, N, State[token]);

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
}