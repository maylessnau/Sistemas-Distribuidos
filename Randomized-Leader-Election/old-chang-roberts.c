/* Autoria: Mayara Lessnau de Figueiredo Neves e ALexander Danila Mion
*  Data da ultima modificação: 26/05/2026
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
#define election 3
#define receive 4
#define leader 5
#define recovery 6

#define UNKNOWN -1
#define CORRETO 0
#define FALHO 1

typedef struct {
    int candidate;      //conteudo da mensagem: candidato a lider
    int origin_id;      //id da origem (pra saber quem mandou)
} Message;

typedef struct {
    int id;             // identificador de facility do SMPL 
    int *State;         // vetor de estados dos processos
    int Lider;          // id do processo considerado lider
    Message *Messages;  // vetor de mensagens recebidas
    int received;       // numero de mensagens recebidas
    int seen;           // numero de mensagens "lidas"
} TipoProcesso;

TipoProcesso *processo;

void limpeza (TipoProcesso* p, int N) {
    for (int i = 0; i < N; i++) {
        free (processo[i].State);
        free (processo[i].Messages);
    }

    free (processo);
    return;
}

void send (int destino, int conteudo, int origem) {
    int id_msg = processo[destino].received;
    processo[destino].Messages[id_msg].candidate = conteudo;
    processo[destino].Messages[id_msg].origin_id = origem;
    schedule (receive, 1 + id_msg, destino);
    processo[destino].received++;
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
           event, i, j,
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

    /******************************* INICIALIZAÇÕES **********************************/
    // inicializando os N processos

    processo = (TipoProcesso*) malloc (sizeof(TipoProcesso)*N);

    for(i = 0; i < N; i++) {
        memset(fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name, 1);
    }

    // inicializando o vetor State[]
    for (i = 0; i < N; i++) {
        processo[i].State = (int*) malloc (sizeof(int) * N);
        for (j = 0; j < N; j++) {
            processo[i].State[j] = UNKNOWN;
        }
        processo[i].State[i] = CORRETO; // processo dono do vetor -> correto
    }

    // inicializando o vetor Messages[]
    // cada processo recebe no maximo N mensagens por rodada de eleicao no Chang-Roberts
    for (i = 0; i < N; i++) {
        processo[i].Messages = (Message*) malloc (sizeof(Message) * N);
    }

    // mostrando vetor State[] em estado inicial
    //for (j = 0; j < N; j++) {
    //    imprime_state(j, N);
    //}

    /********************************** SCHEDULE *************************************/
    // vamos agora fazer o escalonamento dos eventos iniciais

    //for (i = 0; i < N; i++) {
        // todos os processos de 0 ate N-1 vão testar na unidade de tempo 30
    //    schedule(test, 30.0, i); 
    //}

    //schedule(fault, 31.0, 1);
    //schedule(recovery, 61.0, 1);
    
    for (i = 0; i < N; i++) {
        schedule(election, 0, i);
    }

    /*********************************************************************************/

    int mensagens_enviadas = 0;

    // inicializa semente aleatoria
    srand(time(NULL));

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

            case election:
                
                // processo falho nao elege lider
                if (status(processo[token].id) != 0) break;

                int souCandidato = rand() % 2;  // numero aleatorio entre 0 e 1
                int proximo = (token + 1) % N;

                if (souCandidato) {
                    processo[token].Lider = token;
                    send (proximo, token, token);
                    mensagens_enviadas++;
                    printf("O processo %d agora é candidato!\n", token);
                } else  //nao eh candidato
                    processo[token].Lider = -1;

                break;

            case receive:

                proximo = (token + 1) % N;
                int id_msg = processo[token].seen;
                int candidato = processo[token].Messages[id_msg].candidate;
                int origem = processo[token].Messages[id_msg].origin_id;

                printf("Processo %d recebeu mensagem de %d dizendo que %d é candidato a líder.\n", token, origem, candidato);

                if (candidato > processo[token].Lider) {
                    processo[token].Lider = candidato;
                    send (proximo, candidato, token);
                    mensagens_enviadas++;
                    printf ("Processo %d agora considera que %d é o líder!\n", token, candidato);
                }
                else if (candidato == token) {
                    printf ("Fim da Eleição de Líder no tempo %4.1f!\n", time());
                    printf ("Total de mensagens enviadas: %d\n", mensagens_enviadas);
                    // o lider encerra a simulacao
                    schedule (leader, (MaxTempoSimulac - time()), token);
                } else
                    printf ("Mensagem descartada!\n");

                processo[token].seen++;
                break;

            case leader:

                printf ("Eu, processo de ID %d, sou o líder!\n", token);
                break;

            case fault:

                // removi o r = request
                request (processo[token].id, token, 0);
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