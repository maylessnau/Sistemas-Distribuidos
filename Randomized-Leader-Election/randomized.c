/* Autoria: Mayara Lessnau de Figueiredo Neves e Alexander Danila Mion
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
#define newRound 5
#define leader 6
#define recovery 7

#define UNKNOWN -1
#define CORRETO 0
#define FALHO 1

typedef struct {
    int conteudo;       // conteudo da mensagem: bit de candidato
    int origin_id;      // id da origem (pra saber quem mandou)
    int rodada;         // rodada em que a mensagem foi enviada
} Message;

typedef struct {

    int id;             // identificador de facility do SMPL 
    int *State;         // vetor de estados dos processos

    /******************** Variaveis de Eleicao de Lider ***********************/

    int Lider;          // id do processo considerado lider
    short int bit;      // bit de candidato
    int *candidatos;    // conjunto de candidatos a lider
    int *receivedRound; // conjunto de mensagens recebidas na rodada
    int rodadaAtual;    // rodada atual do processo na eleicao


    /******************* Variaveis de Troca de Mensagens **********************/

    Message *Messages;  // vetor de mensagens recebidas
    int received;       // numero de mensagens recebidas
    int seen;           // numero de mensagens "lidas"

} TipoProcesso;

TipoProcesso *processo;

void limpeza (TipoProcesso* p, int N) {
    for (int i = 0; i < N; i++) {
        free (processo[i].State);
        free (processo[i].Messages);
        free (processo[i].candidatos);
        free (processo[i].receivedRound);
    }

    free (processo);
    return;
}

void send (int destino, int conteudo, int origem) {

    int id_msg = processo[destino].received;

    // coloca a mensagem no processo destino
    processo[destino].Messages[id_msg].conteudo = conteudo;
    processo[destino].Messages[id_msg].origin_id = origem;
    processo[destino].Messages[id_msg].rodada = processo[origem].rodadaAtual;

    schedule (receive, 1, destino);
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
    // inicializa semente aleatoria
    srand(time(NULL));
    
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

    // inicializando os vetores Messages[], receivedRound[] e candidatos[] 
    // inicializando o bit aleatorio
    for (i = 0; i < N; i++) {

        processo[i].Messages = (Message*) malloc (sizeof(Message) * N);
        
        processo[i].candidatos = (int*) malloc (sizeof(int) * N);
        processo[i].receivedRound = (int*) malloc (sizeof(int) * N);

        for (j = 0; j < N; j++) {
            processo[i].candidatos[j] = -1;
            processo[i].candidatos[j] = -1;
        }

        processo[i].bit = rand() % 2;
        processo[i].rodadaAtual = 0;
    }

    // mostrando vetor State[] em estado inicial
    //for (j = 0; j < N; j++) {
    //    imprime_state(j, N);
    //}

    /********************************** SCHEDULES ************************************/
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

            // INIT do algoritmo de elicao de lider
            case election:
                
                int proximo = (token + 1) % N;

                printf ("Processo %d tem bit %d na rodada %d\n", token, 
                        processo[token].bit, processo[token].rodadaAtual);

                send (proximo, processo[token].bit, token);
                mensagens_enviadas++;

                break;

            case receive:

                proximo = (token + 1) % N;
                int id_msg = processo[token].seen;
                int bit = processo[token].Messages[id_msg].conteudo;
                int origem = processo[token].Messages[id_msg].origin_id;

                int rodada_msg = processo[token].Messages[id_msg].rodada;

                // ignora mensagens antigas
                if (rodada_msg < processo[token].rodadaAtual) {
                    processo[token].seen++;
                    break;
                }

                // atualiza a rodada (se for preciso)
                processo[token].rodadaAtual = rodada_msg;

                // o processo eh candidato e nao eh considerado candidato
                if (bit == 1 && processo[token].candidatos[origem] == -1) {
                    processo[token].candidatos[origem] = origem;  
                    printf("Processo %d recebeu mensagem dizendo que %d é candidato a líder.\n", token, origem);
                } else {
                    printf("Processo %d recebeu mensagem dizendo que %d não é candidato a líder.\n", token, origem);
                }

                // se a mensagem voltou (o anel garante que a mensagem passou por todos os corretos)
                if (origem == token) {
                    printf ("Processo %d terminou rodada\n", token);
                    schedule(newRound, 1, token);
                } else {
                    send (proximo, bit, origem);
                    mensagens_enviadas++;
                }

                processo[token].seen++;
                break;

            case newRound:
            
                processo[token].rodadaAtual++;

                int total = 0;
                int possivel_lider = -1;
                proximo = (token + 1) % N;

                for (i = 0; i < N; i++) {
                    if (processo[token].candidatos[i] != -1) {
                        total++;
                        possivel_lider = i;
                    }
                }

                // restou apenas um processo candidato -> ele eh o lider
                if (total == 1) {
                    printf ("Fim da Eleição de Líder no tempo %4.1f!\n", time());
                    processo[token].Lider = possivel_lider;
                    schedule (leader, 1, possivel_lider);
                    printf ("O processo %d reconhece %d como líder\n", token, possivel_lider);
                    break;
                } 

                // nao tem lider -> proxima rodada
            
                // limpa estruturas da rodada anterior
                for (i = 0; i < N; i++) {
                    processo[token].candidatos[i] = -1;
                }

                processo[token].received = 0;
                processo[token].seen = 0;

                // acabaram os candidatos :( ou o processo pode sortear de novo
                if (total == 0 || processo[token].bit == 1) {
                    // todos participam de novo
                    processo[token].bit = rand() % 2;
                    send (proximo, processo[token].bit, token);
                    mensagens_enviadas++;
                }

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