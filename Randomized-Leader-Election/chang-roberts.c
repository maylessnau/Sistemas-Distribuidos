#include "chang-roberts.h"

// inicializa as estruturas que vao ser utilizadas pelo algoritmo
void cr_init(ProcessoCR processo[], int N){

    int i, j;

    for (i = 0; i < N; i++) {
        // no comeco nao tem lider
        processo[i].Lider = -1;
        processo[i].Messages = fila_cria();
        processo[i].State = (int*) malloc (sizeof(int) * N);
        for (j = 0; j < N; j++) {
            processo[i].State[j] = UNKNOWN;
        }
        processo[i].State[i] = CORRETO; // processo dono do vetor -> correto
    }
}

// inicia a elicao de lider para um processo (token)
void cr_election(ProcessoCR processo[], int token, int N, int rodadaAtual, 
    int *mensagensEnviadas, int *comecaramEleicao);

// trata o recebimento de uma mensagem
void cr_receive(ProcessoCR processo[], int token, int N, int rodadaAtual, 
    int *mensagensEnviadas);

// finaliza a eleicao de lider
void cr_leader (int token, int mensagensEnviadas);

// envia uma mensagem para outro processo do anel
void cr_send(ProcessoCR processo[], int destino, int bit, int origem, int rodada);

// limpa a memoria utilizada
void cr_destroy(ProcessoCR processo[], int N);