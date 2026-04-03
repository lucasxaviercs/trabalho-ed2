#pragma once

#include "structs.h"
#include "utils.h"

void IgnorarLinhaZeroCSV(FILE *arquivoCSV);

Header InicializarCabecalho();

Header LerCabecalhoBIN(FILE *arquivoBIN, const Header cabecalho);

Registro LerRegistroBIN(FILE *arquivoBIN, Registro registroDados);

Registro LerRegistroCSV(FILE *arquivoCSV, Registro registroDados);

void EscreverCabecalhoBIN(FILE* arquivoBIN, const Header* cabecalho);

void EscreverRegistroBIN(FILE* arquivoBIN, const Registro* registroDados);

void LiberarStringRegistro(Registro *registroDados);