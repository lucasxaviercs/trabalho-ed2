#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>

#define TAM_CABECALHO 17
#define TAM_REGISTRO 80

typedef struct {
    char status; // indica se o arquivo está consistente ('1') ou inconsistente ('0')
    int topo; // armazena o RRN de um registro logicamente removido ou '-1' caso não haja nenhum registro logicamente removido
    int proxRRN; // próximo RRN disponível
    int nroEstacoes; // indica a quantidade de estações diferentes no arquivo de dados
    int nroParesEstacao; // indica a quantidade de pares (codEstacao, codProxEstacao) 
} Header;

typedef struct {
    char removido; // indica se o registro foi logicamente removido ou não
    int proximo; // armazena o RRN do próximo registro logicamente removido
    int codEstacao; // código da estação
    int codLinha; // código da linha
    int codProx; // código da próxima estação
    int Estacao; // 
    int distProxEstacao; // distância para a próxima estação
    int codLinhaIntegra; // código da linha que faz a integração entre as linhas
    int codEstIntegra; // código da estação que faz a integração entre as linhas
    int tamNomeEstacao; // tamanho do campo nomeEstacao
    char nomeEstacao[tamNomeEstacao]; // nome da estação
    int tamNomeLinha; // tamanho do campo nomeLinha
    char nomeLinha[tamNomeLinha]; // nome da linha
} Registro;