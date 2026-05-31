#ifndef _CHANGROBERTS_H
#define _CHANGROBERTS_H

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

// casos de log
#define UM_CANDIDATO      1
#define VARIOS_CANDIDATOS 2
#define TODOS_CANDIDATOS  3

// struct do processo para o algoritmo randomized
typedef struct {

    int id;                     // identificador de facility do SMPL 
    int *State;                 // vetor de estados dos processo

    /********************* Variaveis de Eleicao de Lider ************************/

    int Lider;                  // id do processo considerado lider
 
    /*************************** Troca de Mensagens ***************************/

    struct fila *Messages;      // fila de mensagens recebidas

} ProcessoCR;

// inicializa as estruturas que vao ser utilizadas pelo algoritmo
void cr_init (ProcessoCR processo[], int N);

// inicia a elicao de lider para um processo (token)
void cr_election (ProcessoCR processo[], int token, int N, int *mensagensEnviadas, 
    int modo);

// trata o recebimento de uma mensagem
void cr_receive (ProcessoCR processo[], int token, int N, int *mensagensEnviadas);

// envia uma mensagem para outro processo do anel
void cr_send (ProcessoCR processo[], int destino, int bit, int origem, int N);

// limpa a memoria utilizada
void cr_destroy (ProcessoCR processo[], int N);

#endif