#include <stdio.h>
#include <stdlib.h>

#include "funcionalidades.h"

int main(int argc, char *argv[]){
    int funcionalidade;

    char *arquivoEntrada = malloc(100 * sizeof(char));
    char *arquivoSaida = malloc(100 * sizeof(char));

    if(scanf("%d", &funcionalidade) != 1) return 0;

    switch (funcionalidade) {
        case CREATE_TABLE: 
            scanf("%s %s", arquivoEntrada, arquivoSaida);
            CreateTable(arquivoEntrada, arquivoSaida);
            break;

        case SELECT_FROM:
            scanf("%s", arquivoEntrada);
            SelectFrom(arquivoEntrada);
            break;

        case SELECT_WHERE:
        {
            int nroBuscas = 0;
            scanf("%s %d", arquivoEntrada, &nroBuscas);
            SelectWhere(arquivoEntrada, nroBuscas);
            break;
        }
        case RECUPERACAO_RRN:
        {
            int RRN;
            scanf("%s %d", arquivoEntrada, &RRN);
            RecuperacaoRRN(arquivoEntrada, RRN);
            break;
        }
        
        default:
            printf("Funcionalidade inválida.\n");
            break;
    }

    free(arquivoEntrada);
    arquivoEntrada = NULL;
    free(arquivoSaida);
    arquivoSaida = NULL;

}