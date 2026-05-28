#include "randomized.h"

void rand_init(ProcessoRand processo[], int N) {

    int i, j;

    // percorre todos os processo
    for (i = 0; i < N; i++) {

        processo[i].candidatos = malloc(sizeof(int) * N);
        processo[i].receivedRound = malloc(sizeof(int) * N);
        processo[i].participantesRodada = malloc(sizeof(int) * N);

        for (j = 0; j < N; j++) {

            // -1 indica que nao tem nada
            processo[i].candidatos[j] = -1;
            processo[i].receivedRound[j] = -1;

            // no inicio todos participam
            processo[i].participantesRodada[j] = j;
        }          

        processo[i].Lider = -1;
        processo[i].bit = 0;
        processo[i].rodadaTerminada = 0;
        processo[i].Messages = fila_cria();
    }

    // inicializando o vetor State[] de cada processo
    for (i = 0; i < N; i++) {
        processo[i].State = (int*) malloc (sizeof(int) * N);
        for (j = 0; j < N; j++) {
            processo[i].State[j] = UNKNOWN;
        }
        processo[i].State[i] = CORRETO; // processo dono do vetor -> correto
    }
}

void rand_election(ProcessoRand processo[], int token, int N, int rodadaAtual, 
    int *mensagensEnviadas, int *comecaramEleicao) {
    
    // calcula quem eh o proximo no anel
    int proximo = (token + 1) % N;

    // mostra o bit inicial no log
    printf(
        "Processo %d tem bit inicial = %d\n",
        token,
        processo[token].bit
    );

    // quando todos tiverem começado imprime 
    if (++(*comecaramEleicao) == N) {

        printf(
            "\n================== "
            "INICIANDO PRIMEIRA RODADA [%d] "
            "===================\n\n",
            rodadaAtual
        );
    }

    // envia o bit inicial para o proximo do anel
    rand_send(processo, proximo, processo[token].bit, token, rodadaAtual);
    (*mensagensEnviadas)++;
}

void rand_receive (ProcessoRand processo[], int token, int N, int rodadaAtual, 
    int *mensagensEnviadas) {
    
    int i;
    int proximo = (token + 1) % N;

    struct mensagem msg;

    /************************ RECEBE MENSAGEM ************************/

    // tira a mensagem da fila e verifica se ela existe
    if (!fila_dequeue(processo[token].Messages, &msg)) {

        printf("[ERRO] Processo %d tentou receber mensagem inexistente.\n", token);
        return;
    }

    int bit = msg.bit;
    int origem = msg.origem;
    int rodada = msg.rodada;

    // ignora mensagens de rodadas passadas
    if (rodada != rodadaAtual) {
        
        printf("[ERRO] Processo %d recebeu mensagem de outra rodada.\n", token);
        return;
    }

    // marca que recebeu mensagem do processo origem
    processo[token].receivedRound[origem] = 1;

    /**************************** PROCESSA MENSAGEM ****************************/

    // se eh uma candidatura
    if (bit == 1) {

        if (processo[token].candidatos[origem] == -1) {

            processo[token].candidatos[origem] = origem;
            printf("Processo %d recebeu candidatura de %d.\n", token, origem);
        }
    // eh uma desistencia
    } else
        printf("Processo %d recebeu desistência de %d.\n", token, origem);

    // se voltou ao dono -> remove do anel
    if (origem == token) {

        printf("Processo %d removeu sua mensagem do anel.\n", token);

    } else {
        // se a mensagem nao eh minha, vou repassar
        rand_send(processo, proximo, bit, origem, rodada);
        (*mensagensEnviadas)++;
    }

    /************************** VERIFICA FIM DA RODADA **************************/

    // toda vez que recebe uma mensagem, pode ser que a rodada tenha acabado

    int terminou = 1;
    i = 0;

    while (i < N && terminou) {

        // se o processo participa da rodada
        if (processo[token].participantesRodada[i] != -1) {

            // mas ainda não recebi mensagem dele
            if (processo[token].receivedRound[i] == -1)
                terminou = 0;
        }
        i++;
    }

    if (terminou && processo[token].rodadaTerminada < rodadaAtual) {

        // guarda o valor da ultima rodada terminada
        processo[token].rodadaTerminada = rodadaAtual;
        schedule(newRound, 1.0, token);
    }
}

void rand_new_round(ProcessoRand processo[], int token, int N, int *rodadaAtual,
    int *ultimaRodadaAtualizada, int *eleicaoTerminou, int *mensagensEnviadas) {

    int i;
    int proximo = (token + 1) % N;

    int total = 0;
    int candidatoRestante = -1;

    // verificando se todos os processo já terminaram a rodada
    int todosTerminaram = 1;
    i = 0;

    while (i < N && todosTerminaram) {

        // se o processo participa da rodada
        if (processo[token].participantesRodada[i] != -1) {

            // mas ainda nao esta na nova rodada
            if (processo[i].rodadaTerminada < *rodadaAtual) 
                todosTerminaram = 0;
        }
        i++;
    }

    // conta quantos candidatos tem atualmente
    for (i = 0; i < N; i++) {
        if (processo[token].candidatos[i] != -1) {
            total++;
            candidatoRestante = i;
        }
    }

    /****************************** VERIFICA SE TEM LIDER ******************************/

    // se total = 1 entao resta apenas 1 candidato, o lider!
    if (total == 1) {

        processo[token].Lider = candidatoRestante;

        // controle para nao ter varios processo avisando que a eleicao terminou
        if (!(*eleicaoTerminou) && todosTerminaram) {

            *eleicaoTerminou = 1;

            printf("\nFim da Eleição de Líder no tempo %4.1f!\n", time());

            printf("\nLíder eleito: processo %d\n", candidatoRestante);

            schedule(leader, 1.0, candidatoRestante);
        }

        return;
    }

    // nao tem lider :((

    // atualiza a rodada (vai precisar de mais)
    if (todosTerminaram && *ultimaRodadaAtualizada < *rodadaAtual) {

        *ultimaRodadaAtualizada = *rodadaAtual;

        printf("\nFim da Rodada!\n");

        printf("Candidatos restantes: [");

        // logica pra imprimir eles separados por ,
        int primeiro = 1;

        for (i = 0; i < N; i++) {

            if (processo[token].candidatos[i] != -1) {

                if (!primeiro)
                    printf(", ");

                printf("%d", processo[token].candidatos[i]);
                primeiro = 0;
            }
        }

        printf("]\n");

        (*rodadaAtual)++;

        printf("\n================== INICIANDO NOVA RODADA [%d] ==================\n\n",
               *rodadaAtual);
    }

    /************************ DETERMINA OS PARTICIPANTES ************************/

    if (total == 0) {

        // ninguém sobrou -> todos voltam a participar
        for (i = 0; i < N; i++)
            processo[token].participantesRodada[i] = i;

    } else {

        // só candidatos continuam
        for (i = 0; i < N; i++)
            processo[token].participantesRodada[i] = processo[token].candidatos[i];
    }

    /************************ SORTEIO DO NOVO BIT ************************/

    // se o processo participa dessa rodada
    if (processo[token].participantesRodada[token] != -1) {

        // sorteia o novo bit
        processo[token].bit = rand() % 2;

        printf("Processo %d sorteou novo bit = %d\n", token, processo[token].bit);

        // envia o novo bit para o proximo no anel
        rand_send(processo, proximo, processo[token].bit, token, *rodadaAtual);
        (*mensagensEnviadas)++;
    }

    // limpando as estruturas pra nova rodada
    for (i = 0; i < N; i++) {
        processo[token].candidatos[i] = -1;
        processo[token].receivedRound[i] = -1;
    }
}

void rand_leader (int token, int mensagensEnviadas) {
      
    printf ("Mensagens enviadas: %d\n", mensagensEnviadas);
    printf ("\nEu, processo de ID %d, sou o líder!\n", token);
    return;

}

void rand_send(ProcessoRand processo[], int destino, int bit, int origem, int rodada) {

    struct mensagem msg;

    msg.bit = bit;
    msg.origem = origem;
    msg.rodada = rodada;

    // coloca a mensagem na fila de mensagens do processo destino
    fila_enqueue(processo[destino].Messages, msg);

    // escalona um evento pro destino tratar a mensagem
    schedule(receive, 1.0, destino);
}

void rand_destroy(ProcessoRand processo[], int N) {

    int i;

    // limpa para todos os processos
    for (i = 0; i < N; i++) {

        free(processo[i].candidatos);

        free(processo[i].receivedRound);

        free(processo[i].participantesRodada);

        fila_destroi(&processo[i].Messages);
    }
}