//finalidade: aprender a programar a simulação de algoritmos distribuídos 
//data: 02 de abril de 2026

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

// vamos definir os eventos: um processo em algum instante de tempo pode sofrer um evento
#define test 1
#define fault 2
#define recovery 3

// em seguida, vamos definir o descritor do processo

typedef struct {
    int id;     //identificador de facility do SMPL 
    // (facility: objeto que está simulando, no nosso caso são os processos)
    // outras variáveis locais de cada processo são declaradas aqui dentro
} TipoProcesso;

TipoProcesso *processo;

int main (int argc, char *argv[]) {

    static int N, //número de processos do sistema distribuído
           token, //indica o processo o processo que está executando agora
           event, r, i,
           MaxTempoSimulac = 120;
    static char fa_name[5]; // nome da facility

    if (argc != 2) {
        puts("Uso correto: tempo <numero de processos>");
        exit(1);
    }

    N = atoi(argv[1]);

    smpl(0, "Meu primeiro programa de simulacao de sistemas distribuidos");
    reset();
    stream(1);

    // inicializar os N processos

    processo = (TipoProcesso*) malloc (sizeof(TipoProcesso)*N);

    for(i = 0; i < N; i++) {
        memset(fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name, 1);
    }

    // Vamos agora fazer o escalonamento dos eventos iniciais
    // No primeiro intervalo de testes os processos vão testar

    for (i = 0; i < N; i++) {
        // todos os processos de 0 até N-1 vão testar na unidade de tempo 30
        schedule(test, 30.0, i); 
    }

    schedule(fault, 31.0, 1);
    schedule(recovery, 61.0, 1);

    // agora vem o loop principal do simulador

    while (time() < MaxTempoSimulac) {

        cause(&event, &token);
        switch(event) {
            case test: 
                if (status(processo[token].id) != 0) break; //processo falho não testa
                printf ("Sou o processo %d e estou testando no tempo %4.1f\n", token, time());
                schedule (test, 30.0, token);
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
}

