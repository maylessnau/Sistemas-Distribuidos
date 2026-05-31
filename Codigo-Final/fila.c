/*
 * TAD pilha
 * -> versao com lista ligada com nodo cabeca
 * Trabalho Pratico 5 da disciplina de Programacao 1
 * Autora:
 *    Mayara Lessnau de Figueiredo Neves
 * 
 * Adapatada por: 
 *    Alexander Danila Mion
 *    Mayara Lessnau de Figueiredo Neves
 * 
 * Esta adaptacao tem a finalidade de comportar uma struct
 * de mensagens que sao trocadas por processos dentro da fila.
 * 
 * Versao 3.0.1 de 20/05/2026
 */

#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

struct fila *fila_cria () {

	struct fila *novaFila;
	novaFila = (struct fila *) malloc (sizeof (struct fila));
    
	/* Verifica se a alocacao foi bem sucedida ou nao. */
	if (novaFila == NULL)
		return NULL;

    /* Inicialmente o inicio e fim da fila apontam para NULL, até que algum 
	 * elemento seja inserido.                                           */	
	novaFila->ini = NULL;
	novaFila->fim = NULL;
	novaFila->tamanho = 0;

	return novaFila;
}

void fila_destroi (struct fila **fila) {

	struct nodo *aux;
	struct mensagem *dado;

	aux = (*fila)->ini;

	/* Enquanto a fila nao estiver vazia. */
	while (aux != NULL) {
		dado = &(*fila)->ini->chave; 
		fila_dequeue (*fila, dado);
		aux = (*fila)->ini;
	}

	free (*fila);
}

int fila_enqueue (struct fila *fila, struct mensagem dado) {

	struct nodo *novoEl;

    /* Aloca memoria para o novo elemento verificando se a alocacao foi bem 
	 * sucedida ou nao.                                                  */	
	if (! (novoEl = (struct nodo *) malloc (sizeof (struct nodo))))
		return 0;

	novoEl->chave = dado;
	novoEl->prox = NULL;

    /* Se estamos inserindo o primeiro elemento da fila movemos o inicio. */
    if (fila->ini == NULL)  
		fila->ini = novoEl;

    /* O que era antes o ultimo elemento da fila agora aponta para o novo 
	 * elemento.                                                       */
    else
		fila->fim->prox = novoEl;

	fila->fim = novoEl;
	fila->tamanho++;

	return 1;
}

int fila_dequeue (struct fila *fila, struct mensagem *dado) {

	struct nodo *aux;
	aux = fila->ini;

    /* Se a fila estiver vazia. */
	if (fila == NULL || aux == NULL) 
		return 0;

    /* Deslocamos o inicio da fila para o proximo elemento. */
	fila->ini = aux->prox;

	*dado = aux->chave;
    aux->prox = NULL;

	free (aux);
	aux = NULL;

    /* Se este eh o ultimo elemento, o fim tambem deve apontar para NULL.*/
	if (fila->ini == NULL) 
		fila->fim = NULL;

	fila->tamanho--;

	return 1;
}

int fila_tamanho (struct fila *fila) {

	return fila->tamanho;
}

int fila_vazia (struct fila *fila) {

 	if (fila->tamanho != 0)
		return 0;
	return 1;
}