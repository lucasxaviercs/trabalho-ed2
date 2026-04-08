#pragma once

    #include "structs.h"
    #include "utils.h"


    void IgnorarLinhaZeroCSV(FILE *arquivoCSV);

    Header *InicializarCabecalho();

    void LerCabecalhoBIN(FILE *arquivoBIN, Header *cabecalho);
    void LerRegistroBIN(FILE *arquivoBIN, Registro *registroDados);
    void LerRegistroCSV(FILE *arquivoCSV, Registro *registroDados);

    void EscreverCabecalhoBIN(FILE* arquivoBIN, const Header* cabecalho);
    void EscreverRegistroBIN(FILE* arquivoBIN, const Registro* registroDados);