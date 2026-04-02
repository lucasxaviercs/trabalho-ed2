#pragma once

#include "structs.h"
#include "utils.h"

Header InicializarCabecalho(FILE *arquivoCSV);

Header LerCabecalhoBIN(FILE *arquivoBIN, const Header cabecalho);

Registro LerRegistroCSV(FILE *arquivoCSV, Registro registroDados);

void EscreverCabecalhoBIN(FILE* arquivoBIN, const Header* cabecalho);

void EscreverRegistroBIN(FILE* arquivoBIN, const Registro* registroDados);

void LiberarStringRegistro(Registro *registroDados);