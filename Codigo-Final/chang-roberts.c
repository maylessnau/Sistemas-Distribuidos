#include "chang-roberts.h"

// encontra o proximo processo correto
int proximo_correto (ProcessoCR processo[], int token, int N) {

    int p = (token + 1) % N;

    while (p != token) {

        if (processo[token].State[p] != SUSPEITO)
            return p;

        p = (p + 1) % N;
    }

    return token;
}

// inicializa as estruturas que vao ser utilizadas pelo algoritmo
void cr_init(ProcessoCR processo[], int N){

    int i, j;

    for (i = 0; i < N; i++) {

        // no comeco nao tem lider
        processo[i].Lider = -1;
        processo[i].Messages = fila_cria();

        // inicializacao do vetor State []
        processo[i].State = (int*) malloc (sizeof(int) * N);

        for (j = 0; j < N; j++) {
            processo[i].State[j] = UNKNOWN;
        }

        processo[i].State[i] = CORRETO; // processo dono do vetor -> correto
    }
}

// inicia a elicao de lider para um processo (token)
void cr_election(ProcessoCR processo[], int token, int N, int *mensagensEnviadas, 
    int modo) {

    int souCandidato = 0;  // numero aleatorio entre 0 e 1
    
    switch (modo) {

    case UM_CANDIDATO:
        
        int maiorCorreto = -1;

        for (int i = N - 1; i >= 0; i--) {

            if (status(processo[i].id) == 0) {
                maiorCorreto = i;
                break;
            }
        }

        souCandidato = (token == maiorCorreto);
        
        break;

    case VARIOS_CANDIDATOS:

        if (status(processo[token].id) == 0) 
            souCandidato = rand() % 2;
        
        break;

    case TODOS_CANDIDATOS:

        if (status(processo[token].id) == 0) 
            souCandidato = 1;

        break;
    }

    int proximo = proximo_correto(processo, token, N);

    if (souCandidato) {

        processo[token].Lider = token;

        cr_send (processo, proximo, token, token, N);
        (*mensagensEnviadas)++;

        printf("O processo %d agora é candidato!\n", token);

    } else  //nao eh candidato
        processo[token].Lider = -1;
}

// trata o recebimento de uma mensagem
void cr_receive (ProcessoCR processo[], int token, int N, int *mensagensEnviadas) {
    
    struct mensagem msg;

    if (!fila_dequeue(processo[token].Messages, &msg))
        return;

    int proximo = proximo_correto (processo, token, N);

    int candidato = msg.bit;

    if (status(processo[candidato].id) != 0) {

         printf("Candidato %d está falho. Nova eleição.\n",
           candidato);

        // como um candidato falhou, precisa recomecar a eleicao
        for (int i = 0; i < N; i++) {
            if (status(processo[i].id) == 0)
                schedule(election, 1.0, i);
        }

        return;
    }
    
    int origem = msg.origem;

    printf("[Tempo:%4.1f] Processo %d recebeu mensagem de %d dizendo que %d é candidato a líder.\n",
        time(), token, origem, candidato);

    if (candidato > processo[token].Lider) {
       
        processo[token].Lider = candidato;

        cr_send(processo, proximo, candidato, token, N);

        (*mensagensEnviadas)++;

        printf("Processo %d agora considera que %d é o líder!\n",
            token, candidato);

    } else if (candidato == token) {

        printf("Fim da Eleição de Líder no tempo %4.1f!\n", time());
        schedule(leader, 1.0, token);

    } else
        printf("Mensagem descartada!\n");
}

// envia uma mensagem para outro processo do anel
void cr_send(ProcessoCR processo[], int destino, int candidato, int origem, int N) {
   
    struct mensagem msg;

    msg.origem = origem;
    msg.bit = candidato;
    // nao precisamos da rodada aqui

    while (status(processo[destino].id) != 0) {

        processo[origem].State[destino] = SUSPEITO;

        destino = (destino + 1) % N;

        if (destino == origem)
            return;
    }

    fila_enqueue(processo[destino].Messages, msg);

    schedule(receive, 1.0, destino);
}

// limpa a memoria utilizada
void cr_destroy(ProcessoCR processo[], int N) {
    
    int i;

    for (i = 0; i < N; i++) {
        free(processo[i].State);
        fila_destroi(&processo[i].Messages);
    }
}