#include "funcionalidades.h"


void CreateTable(char *arquivoEntrada, char *arquivoSaida){
    // Abrindo arquivo CSV para leitura
    FILE *arquivoCSV = fopen(arquivoEntrada, "r");
    // Abortando funcionalidade caso ocorra erro na abertura do arquivo CSV
    if(arquivoCSV == NULL){
        MensagemErro();
        fclose(arquivoCSV);
        return;
    }

    // Abrindo arquivo BIN para escrita binária
    FILE *arquivoBIN = fopen(arquivoSaida, "wb");
    // Abortando funcionalidade caso ocorra erro na abertura do arquivo BIN
    if(arquivoBIN == NULL){
        MensagemErro();
        fclose(arquivoBIN);
        return;
    }

    // INICIALIZAÇÃO DE ESTRUTURAS NA MEMÓRIA PRIMÁRIA
    // inicialiamos os valores do cabeçalho
    Header cabecalho = InicializarCabecalho(arquivoCSV);

    // inicializamos estruturas auxiliares que nos vão ajudar a determinar nroEstacoes e nroParesEstacoes
    ControleEstacoes *controleEstacoes = InicializarControleEstacoes();
    ControlePares *controlePares = InicializarControlePares

    // "RESERVAMOS" OS PRIMEIROS 17 BYTES (0-16) DO ARQUIVO BINÁRIAO PARA O CABEÇALHO
    EscreverCabecalhoBIN(arquivoBIN, &cabecalho);


    // LOOP PARA PROCESSAR OS DADOS DO ARQUIVO DE ENTRADA
    // vamos obter os registros de dados do arquivo CSV e escrever eles no arquivo binário
    while(VerificaEOF(arquivoCSV)){

        Registro registroDados = LerRegistroCSV(arquivoCSV, registroDados);
        if(registroDados.removido == '1'){ // se o registro está logicamente removido
            LiberarStringRegistro(&registroDados);
            break;
        }
        
        RegistrarEstacaoUnica(controleEstacoes, registroDados.nomeEstacao);
        RegistrarParUnico(controlePares, registroDados.codEstacao, registroDados.codProxEstacao);

        EscreverRegistroBIN(arquivoBIN, &registroDados);

        cabecalho.proxRRN++;

        LiberarStringRegistro(&registroDados);
    }

    //ATUALIZACAO FINAL DO CABECALHO
    cabecalho.nroEstacoes = controleEstacoes->totalEstacoesUnicas;
    cabecalho.nroParesEstacao = controlePares->totalParesUnicos;
    cabecalho.status = '1'; // Finalizando o uso do arquivo
    EscreverCabecalhoBIN(arquivoBIN, &cabecalho); // O fseek dentro da função nos garante que irá sobrescrever os primeiro 17 bytes que é referente ao cabeçalho mesmo

    //REALIZANDO OS ÚLTIMOS DESALOCAMENTO DE MEMÓRIA E FECHANDO OS ARQUIVOS
    LiberarControleEstacoes(controleEstacoes);
    LiberarControlePares(controlePares);

    fclose(arquivoCSV);
    fclose(arquivoBIN);

    // Como exigido no PDF do trabalho
    BinarioNaTela(arquivoSaida);

}