/* Programa: tarefa4.c
 * Finalidade: Fazer cada um dos processos testar o seguinte no anel. Implemente o teste com a função status() do SMPL e imprimir (printf) o resultado de cada teste executado. Por exemplo: “O processo i testou o processo j correto no tempo tal.” 
 * Data: 14 de maio de 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

// Vamos definir os eventos: um processo em algum instante do tempo pode sofrer um evento
#define test 1
#define fault 2
#define recovery 3

// facility deve ser traduzida como recurso
typedef struct {
    int id; //identificador de facility do SMPL
    int *State;     // vetor de estados dos processos
} TipoProcesso;

TipoProcesso *processo;

void imprime_state (int p, int N) {
    int j = 0;

    printf("Vetor de estados PROCESSO %d no tempo %4.1f: [ ", p, time());
    for (j = 0; j < N; j++)
    {
        printf("%d", processo[p].State[j]);
        if (j < N - 1)
            printf(", ");
        else
            printf(" ]\n");
    }

    printf("\n");
    return;
}

int main (int argc, char *argv[]) {
    int j = 0;  // var usada para guardar o endereço do próximo depois de token
    int g = 0;  // ainda nao sei pq eh g
	
	static int N, // númers de processos do sistema distribuído
    	token, // indica o processo que está executando
     	event, r, i,
    	MaxTempoSimulac = 120;

    static char fa_name[5]; // nome da facility

    if (argc != 2) {
        puts("Uso correto: ./tempo <numero de processos>");
        exit(1);
    }

    N = atoi(argv[1]);

    //num threads , nome pro programa
    smpl(0, "Meu primeiro programa de simulacao de sistemas distribuidos");
    reset(); // sempre que começa tem que dar reset
    stream(1);

    // inicializar os N processos
    processo = (TipoProcesso*) malloc (sizeof(TipoProcesso)*N);
 
    for(i = 0; i < N; i++) {
        memset(fa_name, '\0', 5);
        sprintf(fa_name, "%d", i);
        processo[i].id = facility(fa_name, 1);

        processo[i].State = (int *)malloc(N * sizeof(int)); // alocar o vetor de estado para cada processo
        for (int j = 0; j < N; j++)
        {
            processo[i].State[j] = -1; // como ele nao conhece o estado de ninguem, todos são unknown
        }
        processo[i].State[i] = 0; // o processo sabe seu proprio estado, que eh correto!!
    }

    for (i = 0; i < N; i++) {
        schedule(test, 30.0, i); 
    }

    // todos falhos em 31, vamos testar!
    //v1: todos corretos

    //v2: 1 e 4 falhos, crash 31 rec 61
    // schedule(fault, 31.0, 1);
    // schedule(recovery, 61.0, 1);
    // schedule(fault, 31.0, 4);
    // schedule(recovery, 61.0, 4);

    // v3: 1 a 6 falhos, crash 31 rec 91
    // for(int i = 1; i < 7; i++) {
    //     schedule(fault, 31.0, i);
    //     schedule(recovery, 61.0, i);
    // }

    //v4: 1 até N falhos (so 0 correto), crash 31 rec 61
    // for(int i = 1; i < N; i++) {
    //     schedule(fault, 31.0, i);
    //     schedule(recovery, 61.0, i);
    // }


    //v5: 0 até N falhos, crash 1, rec 31
    // for(int i = 0; i < N; i++) {
    //     schedule(fault, 1.0, i);
    //     schedule(recovery, 31.0, i);
    // } 

    while (time() < MaxTempoSimulac) {

        cause(&event, &token);
        switch(event) {
            case test: 
                if (status(processo[token].id) == 1) {
                    printf("Aff!! Sou o processo %d e não posso realizar testes pois estou falho no tempo %4.1f!!\n", token, time());
                    break; // entao esta falho!!
                }
                printf ("Oiii!!! Sou o processo %d e estou testando no tempo %4.1f\n", token, time());

                j = (token + 1) % N;

                while(status(processo[j].id) == 1 && j != token) {
                    printf("Nãooo! O processo %d testou o processo %d e ele está falho no tempo %4.1f\n", token, j, time());
                    processo[token].State[j] = 1;
                    j = (j + 1) % N;
                }
                if(j != token) { // se j == token, entao rodou todos os j's e está em si mesmo, logo, todos estão errados!!
                                 // caso o propio token estivesse falho, teria dado break na primeira linha de case test
                    printf("Obaaa!! O processo %d testou o processo %d e ele está correto no tempo %4.1f\n", token, j, time());
                    processo[token].State[j] = 0;
                } else printf("it's over gurizada :(, todos os processos, menos eu (processo %d) xD, estão falhos!!!!\n", token);

                printf("Sou o processo %d e esse são meus estados:\n", token);
                imprime_state(token, N);

                //quando chega nesse trecho do código, há duas opções:
                // 1) o processo token encontrou um correto em j
                // 2) todos os processos, tirando token, estao falhos

                // para a tarefa4, vamos pegar os states de j e passar para o token
                if(j != token) {
                    printf("Vou pegar todos os estados que o processo %d tem salvo passar pra mim (processo %d), vamos ver como fica!!:\n", j, token);
                    g = (j + 1) % N;
                    while(g != token) {
                        processo[token].State[g] = processo[j].State[g];
                        g = (g + 1) % N;
                    }

                    imprime_state(token, N);
                } else {
                    puts("Como todos os processos estão falhos, não vou pegar os estados de ninguém :(\n");
                }

                schedule (test, 30.0, token);
                break;

            case fault:
                r = request (processo[token].id, token, 0);
                printf ("O processo %d falhou no tempo %4.1f\n", token, time());
                break;

            case recovery:
                release (processo[token].id, token);
                printf ("Viva!! O processo %d recuperou no tempo %4.1f\n", token, time());
                schedule (test, 1.0, token);
                break;
        }
        puts("------------------------------FIM DO EVENTO------------------------------\n");
    }
    free(processo);
}