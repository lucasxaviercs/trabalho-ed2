#pragma once

#include "structs.h"
#include "arquivosLerEscrever.h"
#include "utils.h"

#define CREATE_TABLE 1
#define SELECT_FROM 2
#define SELECT_WHERE 3
#define RECUPERACAO_RRN 4


void CreateTable(char *arquivoEntrada, char *arquivoSaida);

void SelectFrom(char *arquivoEntrada);

void SelectWhere(char *arquivoEntrada, char *arquivoSaida);

void RecuperacaoRRN(char *arquivoEntrada, int RRN);
