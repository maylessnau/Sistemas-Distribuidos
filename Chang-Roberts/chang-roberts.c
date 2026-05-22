/* Autoria: Mayara Lessnau de Figueiredo Neves 
*  Data da ultima modificação: 20/05/2026
*  Finalidade: 
*/

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

#define test 1
#define election 2
#define receive 3
#define leader 4
#define fault 5
#define recovery 6

#define UNKNOWN -1
#define CORRETO 0
#define FALHO 1

typedef struct {
    int candidate;          // candidato a lider 
    int origin_id;          // id do processo que enviou a mensagem
} Messages;

typedef struct {
    int id;                 // identificador de facility do SMPL 
    int *State;             // vetor de estados dos processos
    int Lider;              // variavel que indica quem eh o lider
    int received;           // numero de mensagens recebidas
    int seen;               // numero de mensagens que foram "vistas"
    Messages *messages;     // vetor das mensagens de eleicao recebidas

} TipoProcesso;

TipoProcesso *processo;

// liberando a memoria alocada para cada processo
void limpeza (int N) {
    for (int i = 0; i < N; i++) {
        free (processo[i].messages);
        free (processo[i].State);
    }

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

void send (int destination, int candidate, int origin) {
    // a mensagem tem o id do numero de mensagens recebidas pelo destino
    int id_msg = processo[destination].received;

    // atualiza a mensagem no destino ("envia")
    processo[destination].messages[id_msg].candidate = candidate;
    processo[destination].messages[id_msg].origin_id = origin;
    processo[destination].received++;

    // escalona o receive com o tempo do id (para elas "chegarem" na ordem de envio)
    schedule(receive, id_msg + 1, destination);
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

    // inicializacao do vetor de mensagens
    for (i = 0; i < N; i++) {
        processo[i].messages = (Messages*) malloc (sizeof(Messages) * N);
        processo[i].received = 0;
        processo[i].seen = 0;
    }

    // vamos agora fazer o escalonamento dos eventos iniciais

    /******************************** SCHEDULES ******************************** */
    //for (i = 0; i < N; i++) {
        // todos os processos de 0 ate N-1 vão testar na unidade de tempo 30
        //schedule(test, 30.0, i); 
    //}

    //schedule(fault, 31.0, 1);
    //schedule(recovery, 61.0, 1);
    
    for (i = 0; i < N; i++) {
        schedule(election, 00.0, i);
    }
    /**************************************************************************** */
    

    
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
                
                if (status(processo[token].id) != 0) break; // processo falho nao elege lider
                
                //inicializa semente aleatoria
                srand(time(NULL)); 

                int souCandidato =  rand() % 2;
                printf("souCandidato de %d = %d\n", token, souCandidato);

                int proximo = (token + 1) % N;  // id do proximo no anel

                if (souCandidato) {
                    processo[token].Lider = token;
                    // envia a mensagem (token eh candidato e id da origem)
                    send (proximo, token, token);
                    printf("O processo %d agora é candidato!\n", token);
                }
                else //nao eh candidato
                    processo[token].Lider = -1;

                break;

            case receive:

                proximo = (token + 1) % N;
                int id_msg = processo[token].seen;
                int candidato = processo[token].messages[id_msg].candidate;
                printf("Processo %d recebeu mensagem que %d é o novo candidato.\n", token, candidato);

                // se o candidato da mensagem for maior que o lider atual
                if (candidato > processo[token].Lider) {
                    // atualiza o lider
                    processo[token].Lider = candidato;
                    // informa o proximo
                    send (proximo, candidato, token);
                    printf("Eu, processo %d, considero que %d eh o lider!\n", token, candidato);
                }
                else if (candidato == token) {
                    printf ("Fim da eleicao!!\n");
                    schedule (leader, (MaxTempoSimulac - time()), token);
                } else {
                    printf("Mensagem descartada!\n");
                }           

                processo[token].seen++;
                break;

            case leader:

                printf ("Eu, processo %d, sou o lider!!\n", token);
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
    limpeza(N);
}