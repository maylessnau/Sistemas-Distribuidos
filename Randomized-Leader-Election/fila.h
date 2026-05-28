/*
 * TAD pilha
 * -> versao com lista ligada com nodo cabeca
 * 
 * Autores:
 *    Marcos Alexandre Castilho
 *    Luis Carlos Erpen de Bona
 * Adapatada por: 
 *    Alexander Danila Mion
 *    Mayara Lessnau de Figueiredo Neves
 * 
 * Esta adaptacao tem a finalidade de comportar uma struct
 * de mensagens que sao trocadas por processos dentro da fila.
 * Versao 3.0.1 de 20/05/2026
 */

#ifndef _fila_t_H
#define _fila_t_H

struct mensagem {
    int origem;
    int bit;
    int rodada;
};

struct nodo {
    struct mensagem chave;
    struct nodo *prox;
};

struct fila {
    struct nodo *ini;
    struct nodo *fim;
    int tamanho;
};

/* 
 * Cria e retorna uma nova fila.
 * Retorna NULL em caso de erro de alocação.
*/
struct fila *fila_cria ();

/* Desaloca toda memoria da fila e faz fila receber NULL. */
void fila_destroi (struct fila **fila);

/* 
 * Insere dado na fila (politica FIFO). Retorna 1
 * em caso de sucesso e 0 em caso de falha.
*/
int fila_enqueue (struct fila *fila, struct mensagem dado);

/* 
 * Remove dado da fila (politica FIFO) e retorna o elemento 
 * no parametro dado. A funcao retorna 1 em caso de 
 * sucesso e 0 no caso da fila estar vazia.
*/
int fila_dequeue (struct fila *fila, struct mensagem *dado);
 
/* Retorna o numero de elementos da pilha, que pode ser 0. */
int fila_tamanho (struct fila *fila);

/* Retorna 1 se fila vazia, 0 em caso contrario. */ 
int fila_vazia (struct fila *fila);

#endif