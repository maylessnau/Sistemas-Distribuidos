/* Autores: Mayara Lessnau de Figueiredo Neves e Alexander Danila Mion
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
#include "fila.h"

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

    int id;                     // identificador de facility do SMPL 
    int *State;                 // vetor de estados dos processos

    /******************** Variaveis de Eleicao de Lider ***********************/

    int Lider;                  // id do processo considerado lider
    short int bit;              // bit de candidato
    int *candidatos;            // conjunto de candidatos a lider
    int *receivedRound;         // conjunto de mensagens recebidas na rodada
    int *participantesRodada;   // processos que eram candidatos na rodada anterior
    int rodadaTerminada;        // guarda a ultima rodada terminada pelo processo

    /*************************** Troca de Mensagens ***************************/

    struct fila *Messages;      // fila de mensagens recebidas

} TipoProcesso;

TipoProcesso *processo;

void limpeza (TipoProcesso* p, int N) {

    for (int i = 0; i < N; i++) {
        free (processo[i].State);
        free (processo[i].Messages);
        free (processo[i].candidatos);
        free (processo[i].receivedRound);
        free (processo[i].participantesRodada);
        fila_destroi(&processo[i].Messages);
    }

    free (processo);
    return;
}

void send (int destino, int conteudo, int origem, int rodada) {

    struct mensagem msg;
    msg.bit = conteudo;
    msg.origem = origem;
    msg.rodada = rodada;

    fila_enqueue(processo[destino].Messages, msg);

    schedule (receive, 1, destino);

    //printf("[SEND] %d -> %d | bit=%d | rodada=%d\n",
    //       origem, destino, conteudo, rodada);

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
        puts("Uso correto: ./randomized <numero de processos>");
        exit(1);
    }

    N = atoi(argv[1]);

    smpl(0, "Algoritmo Aleatorizado para Eleicao de Lider");
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

    for (i = 0; i < N; i++) {

        // inicializando os vetores 
        processo[i].candidatos = (int*) malloc (sizeof(int) * N);
        processo[i].receivedRound = (int*) malloc (sizeof(int) * N);
        processo[i].participantesRodada = (int*) malloc (sizeof(int) * N);

        processo[i].Messages = fila_cria();

        for (j = 0; j < N; j++) {
            processo[i].candidatos[j] = -1;
            processo[i].receivedRound[j] = -1;

            // inicialmente todos participam
            processo[i].participantesRodada[j] = j;
        }

        // comeca ja com um bit aleatorio
        processo[i].bit = rand() % 2;

        processo[i].rodadaTerminada = 0;
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

    int mensagensEnviadas = 0;      // conta quantas mensagens foram enviadas na eleicao
    int rodadaAtual = 1;            // numero da rodada atual
    int eleicaoTerminou = 0;        // diz se a eleicao ja terminou
    int ultimaRodadaAtualizada = 0; // guarda o valor da ultima rodada atualizada
    int comecaramEleicao = 0;       // numero de processos que ja comecaram a eleicao

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
        
                printf ("Processo %d tem bit inicial = %d\n", token, 
                        processo[token].bit);
                
                // imprime so quando todos tiverem comecado
                if (++comecaramEleicao == N) {
                    printf("\n================== INICIANDO PRIMEIRA RODADA [1] ==================\n\n");
                }

                send (proximo, processo[token].bit, token, rodadaAtual);
                mensagensEnviadas++;
                
                break;

            case receive:

                proximo = (token + 1) % N;
                struct mensagem msg;

                if (!fila_dequeue(processo[token].Messages, &msg)) {
                    printf ("debug mensagem inexistente");
                    break;
                }

                int bit = msg.bit;
                int origem = msg.origem;
                int rodada = msg.rodada;

                /************************* DEBUG *************************/

                // printf(
                //     "[DEBUG] token=%d | origem=%d | bit=%d | rodada=%d\n",
                //     token,
                //     origem,
                //     bit,
                //     rodada
                // );

                // ignora mensagens atrasadas
                if (rodada != rodadaAtual) {

                    // printf(
                    //     "[DEBUG] mensagem atrasada ignorada "
                    //     "(msg=%d atual=%d)\n",
                    //     rodada,
                    //     rodadaAtual
                    // );

                    break;
                }

                /************************* MENSAGEM *************************/

                processo[token].receivedRound[origem] = 1;

                // o processo eh candidato e nao eh considerado candidato
                if (bit == 1) {

                    if (processo[token].candidatos[origem] == -1) {

                        processo[token].candidatos[origem] = origem;  

                        printf(
                            "Processo %d recebeu candidatura de %d.\n",
                            token, 
                            origem
                        );
                    }

                } else {

                    printf(
                        "Processo %d recebeu desistência de %d.\n", 
                        token, 
                        origem
                    );
                }

                /*********************** REPASSE ************************/

                // se a mensagem voltou ao dono nao repassa (deu a volta no anel)
                if (origem == token) {

                    printf(
                        "Processo %d removeu sua mensagem do anel.\n",
                        token
                    );

                } else {

                    send(proximo, bit, origem, rodada);
                    mensagensEnviadas++;
                }
                
                /******************** VERIFICA NEW ROUND *******************/
                // se uma mensagem nova chegou pode ser que a rodada tenha acabado

                int terminou = 1;
                i = 0;

                // verifica se ainda falta receber mensagem de algum processo
                while (i < N && terminou) {
                    
                    // se o processo deveria mandar mensagem essa rodada
                    if (processo[token].participantesRodada[i] != -1) {

                        // mas token ainda nao recebeu mensagem dele
                        if (processo[token].receivedRound[i] == -1) 
                            terminou = 0;
                    }
            
                    i++;
                }

                // recebeu todas as mensagens e ainda nao terminou a rodada
                if (terminou && processo[token].rodadaTerminada < rodadaAtual) {

                    processo[token].rodadaTerminada = rodadaAtual;
                    schedule (newRound, 1, token);
                }
                // else: se nao terminou espera as proximas mensagens

                break;

            case newRound:

                int total = 0;
                int candidatoRestante = -1;
                proximo = (token + 1) % N;

                int todosTerminaram = 1;
                i = 0;
                
                while (i < N && todosTerminaram) {

                    // se o processo participa da rodada
                    if (processo[token].participantesRodada[i] != -1) {
                        // mas ele ainda nao terminou a rodada
                        if (processo[i].rodadaTerminada < rodadaAtual) {
                            todosTerminaram = 0;
                        }
                    }
                    i++;
                }

                // conta candidatos restantes
                for (i = 0; i < N; i++) {
                    if (processo[token].candidatos[i] != -1) {
                        total++;
                        candidatoRestante = i;
                    }
                }

                /***************************** LIDER *******************************/

                // restou apenas um processo candidato -> ele eh o lider
                if (total == 1) {
                    
                    processo[token].Lider = candidatoRestante;

                    // verificacao para declarar o fim da eleicao apenas uma vez
                    if (!eleicaoTerminou) {

                        eleicaoTerminou = 1;
                        printf (
                            "\nFim da Eleição de Líder no tempo %4.1f!\n", 
                            time()
                        );

                        printf (
                            "Líder eleito: processo %d\n\n", 
                            candidatoRestante
                        );

                        printf ("Reconhecimento:\n");

                        schedule (leader, 1, candidatoRestante);
                    }
                    
                    printf (
                        "O processo %d reconhece %d como líder\n", 
                        token, 
                        candidatoRestante
                    );
                    
                    break;

                /************************** NOVA RODADA ****************************/

                // todos terminaram, nao tem lider e a rodada ainda nao atualizou
                } else if (todosTerminaram && ultimaRodadaAtualizada < rodadaAtual) {
                    
                    ultimaRodadaAtualizada = rodadaAtual;
                
                    printf ("\nFim da Rodada!\n");

                    printf("Candidatos restantes: [");

                    // logica para imprimir os candidatos separados por ,
                    int primeiro = 1;

                    for (i = 0; i < N; i++) {

                        if (processo[token].candidatos[i] != -1) {

                            if (!primeiro) printf(", ");

                            printf("%d", processo[token].candidatos[i]);

                            primeiro = 0;
                        }
                    }

                    printf("]\n");

                    // avanca a rodada
                    rodadaAtual++;

                    printf ("\n================== INICIANDO NOVA RODADA [%d] ==================\n\n",  
                            rodadaAtual);
                }


                /************************** PARTICIPANTES **************************/

                // acabaram os candidatos :(
                if (total == 0) {

                    // todos voltam a participar
                    for (i = 0; i < N; i++) 
                        processo[token].participantesRodada[i] = i;

                } else {

                     // somente os candidatos da ultima rodada participam
                    for (i = 0; i < N; i++) 
                        processo[token].participantesRodada[i] = processo[token].candidatos[i];
                }
                
                 /***************************** ENVIO *****************************/

                // o processo participa dessa nova rodada?
                if (processo[token].participantesRodada[token] != -1) {

                    // sorteia o bit de novo
                    processo[token].bit = rand() % 2;

                    printf (
                        "Processo %d sorteou novo bit = %d\n", 
                        token, 
                        processo[token].bit
                    );
                    
                    // manda mensagem informando o proximo
                    send (proximo, processo[token].bit, token, rodadaAtual);

                    mensagensEnviadas++;
                }

                /**************************** LIMPEZA ****************************/
                
                // limpa estruturas da rodada anterior
                for (i = 0; i < N; i++) {
                    processo[token].candidatos[i] = -1;
                    processo[token].receivedRound[i] = -1;
                }

                break;

            case leader:

                printf ("\nMensagens enviadas: %d\n", mensagensEnviadas);
                printf ("Eu, processo de ID %d, sou o líder!\n", token);
                return 0; // fim do programa -> lider eleito

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