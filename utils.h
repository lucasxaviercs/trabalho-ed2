#pragma once

#include "structs.h"


typedef struct ParEstacao {
    int codEstacaoOrigem;
    int codEstacaoDestino;
} ParEstacao;

typedef struct ControleEstacoes {
    char **listaNomesUnicos; // Array para guardar as strings
    int capacidadeMaxima; // Qtd maxima de espaco alocado na memória
    int totalEstacoesUnicas; // Qtd de estacoes validadas e guardadas
} ControleEstacoes;

typedef struct ControlePares {
    ParEstacao *listaParesUnicos; //Array que guarda os trajetos
    int capacidadeMaxima; // Qtd maxima de espaco alocado na memoria 
    int totalParesUnicos; // Qtd de trajelos validados e inseridos
} ControlePares;

ControleEstacoes *InicializarControleEstacoes();

ControlePares *InicializarControlePares();

void RegistrarEstacaoUnica(ControleEstacoes *controleEstacoes, const char *nomeEstacao);

void LiberarControleEstacoes(ControleEstacoes *controleEstacoes);

void RegistrarParUnico(ControlePares *controlePares, int codigoEstacaoOrigem, int codigoEstacaoDestino);

void LiberarControlePares(ControlePares *controlePares);

void ImprimirRegistro(const Registro *registroDados);

void MensagemErro();

void MensagemRegistroNaoEncontrado();

char VerificaEOF(FILE *f);

void BinarioNaTela(char *arquivo);

void ScanQuoteString(char *str);