/* Autores: Mayara Lessnau de Figueiredo Neves e Alexander Danila Mion
*  Data da ultima modificação: 28/05/2026
*  Finalidade: Este codigo eh uma simulacao de um Sistema Distribuido
*  com detector de falhas em anel, onde cada processo executa testes no
*  proximo do anel ate encontrar outro processo correto. Cada processo
*  tambem mantem localmento o vetor State[N]. A entrada do vetor State[j]
*  indica o estado do processo j (UNKNOWN, CORRETO ou SUSPEITO). Quando um 
*  processo correto testa outro processo correto obtem as informacoes do 
*  estado dos demais processos do sistema.
*/

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"
#include "fila.h"
#include "randomized.h"

ProcessoRand *processo;

void imprime_state (int p, int N) {

    printf("State(%d) no tempo %4.1f:\n", p, time());

    for (int j = 0; j < N; j++)
        printf ("State[%d] = %d\n", j, processo[p].State[j]);

    printf("\n");
    return;
}

// deteccao de falhas 
void teste (int token, int N) {

    int proximo = (token + 1) % N; //q eh o proximo processo no anel
    int estado, q;

    // enquanto ainda nao percorreu todos os processos do anel
    while (proximo != token) {

        estado = status(processo[proximo].id);

        // achou um correto :)
        if (estado == 0) {

            //printf ("O processo %d testou o processo %d correto no tempo %4.1f\n", 
            //    token, proximo, time());

            processo[token].State[proximo] = CORRETO;

            q = (proximo + 1) % N; // i recebe o processo que vem depois de proximo

            // enquanto nao passou por todos os processos restantes
            while (q != token) {

                // o processo token obtem info sobre o processo i (que vieram de proximo)
                processo[token].State[q] = processo[q].State[q]; // atualiza com infos de q
                
                // passa pro proximo
                q = (q + 1) % N;
            }

            break;
        }

        // se chegou aqui eh porque esta falho :(
        printf ("O processo %d testou o processo %d falho no tempo %4.1f\n", 
            token, proximo, time());
        
        processo[token].State[proximo] = SUSPEITO;

        proximo = (proximo + 1) % N; // atualiza com o proximo
    }

    // se q == token entao deu uma volta no anel (testou todos falhos)
    if (proximo == token)
        printf ("O processo %d testou todos os processos falhos no tempo %4.1f\n", token, time());

    // mostrando vetor State[]
    imprime_state(token, N);

}

int main (int argc, char *argv[]) {

    static int N,                   // numero de processos do sistema distribuido
           token,                   // indica o processo que esta executando agora
           event,                   // evento da simulacao
           i, j;                    // controle de indices em vetor

    /********************************* INICIALIZACOES ***********************************/
        
    static char fa_name[5]; 
    
    if (argc != 2) {
        puts("Uso correto: ./main <numero de processos>");
        exit(1);
    }

    N = atoi(argv[1]);

    smpl(0, "Algoritmo Aleatorizado para Eleicao de Lider");
    reset();
    stream(1);
   
    // inicializa semente aleatoria                       
    srand(time(NULL));
    
    // inicializando os N processos
    processo = (ProcessoRand*) malloc (sizeof(ProcessoRand)*N);
    rand_init (processo, N);

    for(i = 0; i < N; i++) {
        memset(fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name, 1);
    }

    /********************************** SCHEDULES ************************************/
    // vamos agora fazer o escalonamento dos eventos iniciais

    for (i = 0; i < N; i++) {
        // todos os processos de 0 ate N-1 vão testar na unidade de tempo 30
       schedule(test, 30.0, i); 
    }

    //schedule(fault, 31.0, 1);
    //schedule(recovery, 61.0, 1);
    
    for (i = 0; i < N; i++) {
        // todos vao eleger um lider
        schedule(election, 0, i);
    }

    /*************************** VARIAVEIS DE CONTROLE ***************************/

    int mensagensEnviadas = 0;      // conta quantas mensagens foram enviadas na eleicao
    int rodadaAtual = 1;            // numero da rodada atual
    int eleicaoTerminou = 0;        // diz se a eleicao ja terminou
    int ultimaRodadaAtualizada = 0; // guarda o valor da ultima rodada atualizada
    int comecaramEleicao = 0;       // numero de processos que ja comecaram a eleicao
    int acabou = 0;                 // determina o fim da execucao

    /************************ LOOP PRINCIPAL DO SIMULADOR ************************/
    
    while (!acabou) {

        //pergunta se existe evento pra acontecer com o processo que tem o token
        cause(&event, &token); 

        switch(event) {

            case test:
         
                //processo falho nao testa
                if (status(processo[token].id) != 0) break; 

                // testa se o processo esta falho ou correto e obtem informacoes
                teste (token, N);

                // testa a cada 30 intervalos de tempo
                schedule (test, 30.0, token);
                break;

            case election:
                
                rand_election (processo, token, N, rodadaAtual, &mensagensEnviadas, 
                    &comecaramEleicao);

                break;

            case receive:

                rand_receive (processo, token, N, rodadaAtual, &mensagensEnviadas);
                break;

            case newRound:

                rand_new_round (processo, token, N, &rodadaAtual, &ultimaRodadaAtualizada,
                    &eleicaoTerminou, &mensagensEnviadas);

                break;

            case leader:

                rand_leader (token, mensagensEnviadas);

                // fim do programa -> lider eleito
                acabou= 1;
                break; 

            case fault:

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

    // limpa a memoria
    rand_destroy (processo, N);
    free (processo);

    return 0;
}