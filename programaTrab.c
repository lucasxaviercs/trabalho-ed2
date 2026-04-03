#include <stdio.h>
#include <stdlib.h>

#include "funcionalidades.h"

int main(int argc, char *argv[]) {
    // Lê o argumento da linha de comando para saber qual funcionalidade executar
    int funcionalidade = atoi(argv[1]);

    switch (funcionalidade) {
        case CREATE_TABLE: 
            CreateTable(argv[2], argv[3]);
            break;

        case SELECT_FROM:
            SelectFrom(argv[2]);
            break;

        case SELECT_WHERE:
            printf("Funcionalidade 3 selecionada.\n");
            break;

        case RECUPERACAO_RRN:
        {
            int RRN = atoi(argv[3]);
            RecuperacaoRRN(argv[2], RRN);
            break;
        }
        
        default:
            printf("Funcionalidade inválida.\n");
            break;
    }
}