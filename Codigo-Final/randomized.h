#ifndef _RANDOMIZED_H
#define _RANDOMIZED_H

#include <stdio.h>
#include <stdlib.h>
# include "fila.h"
#include "smpl.h"

// eventos do algoritmo
#define test 1
#define fault 2
#define election 3
#define receive 4
#define newRound 5
#define leader 6
#define recovery 7

// estados dos processo
#define UNKNOWN -1
#define CORRETO 0
#define SUSPEITO 1

// struct do processo para o algoritmo randomized
typedef struct {

    int id;                     // identificador de facility do SMPL 
    int *State;                 // vetor de estados dos processo

    /********************* Variaveis de Eleicao de Lider ************************/

    int Lider;                  // id do processo considerado lider
    short int bit;              // bit de candidato
    int *candidatos;            // conjunto de candidatos a lider
    int *receivedRound;         // conjunto de mensagens recebidas na rodada

    int *participantesRodada;   // processos que eram candidatos na rodada anterior 
                                //(que participam da rodada atual -> sorteiam bit novo) 

    int rodadaTerminada;        // guarda a ultima rodada terminada pelo processo

    /*************************** Troca de Mensagens ***************************/

    struct fila *Messages;      // fila de mensagens recebidas

} ProcessoRand;

// inicializa as estruturas que vao ser utilizadas pelo algoritmo
void rand_init(ProcessoRand processo[], int N);

// inicia a elicao de lider para um processo (token)
void rand_election(ProcessoRand processo[], int token, int N, int rodadaAtual, 
    int *mensagensEnviadas, int *comecaramEleicao);

// trata o recebimento de uma mensagem
void rand_receive(ProcessoRand processo[], int token, int N, int rodadaAtual, 
    int *mensagensEnviadas);

// controla a transicao entre as rodadas do algoritmo
void rand_new_round(ProcessoRand processo[], int token, int N, int *rodadaAtual,
    int *ultimaRodadaAtualizada, int *eleicaoTerminou, int *mensagensEnviadas);

// envia uma mensagem para outro processo do anel
void rand_send(ProcessoRand processo[], int destino, int bit, int origem, int rodada);

// limpa a memoria utilizada
void rand_destroy(ProcessoRand processo[], int N);

#endif