#include "funcionalidades.h"


void CreateTable(char *arquivoEntrada, char *arquivoSaida){
    // Abrindo arquivo CSV para leitura
    FILE *arquivoCSV = fopen(arquivoEntrada, "r");
    // Abortando funcionalidade caso ocorra erro na abertura do arquivo CSV
    if(arquivoCSV == NULL){
        MensagemErro();
        return;
    }

    // Abrindo arquivo BIN para escrita binária
    FILE *arquivoBIN = fopen(arquivoSaida, "wb");
    // Abortando funcionalidade caso ocorra erro na abertura do arquivo BIN
    if(arquivoBIN == NULL){
        MensagemErro();
        return;
    }

    // INICIALIZAÇÃO DE ESTRUTURAS NA MEMÓRIA PRIMÁRIA
    // inicialiamos os valores do cabeçalho
    Header *cabecalho = InicializarCabecalho();

    // inicializamos estruturas auxiliares que nos vão ajudar a determinar nroEstacoes e nroParesEstacoes
    ControleEstacoes *controleEstacoes = InicializarControleEstacoes();
    ControlePares *controlePares = InicializarControlePares();

    // "RESERVAMOS" OS PRIMEIROS 17 BYTES (0-16) DO ARQUIVO BINÁRIAO PARA O CABEÇALHO
    EscreverCabecalhoBIN(arquivoBIN, cabecalho);

    // DESCARTAMOS A LINHA ZERO DO CSV QUE CONTÉM APENAS AS NOMENCLATURAS DAS COLUNAS
    IgnorarLinhaZeroCSV(arquivoCSV);

    // LOOP PARA PROCESSAR OS DADOS DO ARQUIVO DE ENTRADA
    // vamos obter os registros de dados do arquivo CSV e escrever eles no arquivo binário
    while(VerificaEOF(arquivoCSV)){

        Registro registroDados;
        LerRegistroCSV(arquivoCSV, &registroDados);
        if(registroDados.removido == '1'){ // se o registro está logicamente removido
            LiberarStringRegistro(&registroDados);
            break;
        }
        
        // Registra os nomes únicos e pares únicos das estações para contagem do nroEstacoes e nroParesEstacao
        RegistrarEstacaoUnica(controleEstacoes, registroDados.nomeEstacao);
        RegistrarParUnico(controlePares, registroDados.codEstacao, registroDados.codProxEstacao);

        EscreverRegistroBIN(arquivoBIN, &registroDados);

        cabecalho->proxRRN++;

        LiberarStringRegistro(&registroDados);
    }

    //ATUALIZACAO FINAL DO CABECALHO
    cabecalho->nroEstacoes = controleEstacoes->totalEstacoesUnicas;
    cabecalho->nroParesEstacao = controlePares->totalParesUnicos;
    cabecalho->status = '1'; // Finalizando o uso do arquivo

    EscreverCabecalhoBIN(arquivoBIN, cabecalho); // O fseek dentro da função nos garante que irá sobrescrever os primeiro 17 bytes que é referente ao cabeçalho mesmo

    //REALIZANDO OS ÚLTIMOS DESALOCAMENTO DE MEMÓRIA E FECHANDO OS ARQUIVOS
    LiberarControleEstacoes(controleEstacoes);
    LiberarControlePares(controlePares);

    free(cabecalho);
    cabecalho = NULL;

    fclose(arquivoCSV);
    fclose(arquivoBIN);

    // Como exigido no PDF do trabalho
    BinarioNaTela(arquivoSaida);

}

void SelectFrom(char *arquivoEntrada){
    // Abrindo arquivo BIN para leitura binária
    FILE *arquivoBIN = fopen(arquivoEntrada, "rb");
    // Abortando funcionalidade caso ocorra erro na abertura do arquivo BIN
    if(arquivoBIN == NULL){
        MensagemErro();
        return;
    }    

    // Leitura do cabecalho do arquivo binário
    Header cabecalho;
    LerCabecalhoBIN(arquivoBIN, &cabecalho);

    if(cabecalho.status == '0'){ //arquivo inconsistente 
        MensagemErro();
        fclose(arquivoBIN);
        return;
    }

    if(cabecalho.proxRRN == 0){ // não há nenhum dado gravado, apenas o arquivo foi criado
        MensagemRegistroNaoEncontrado();
        fclose(arquivoBIN);
        return;
    }

    int contadorImpressos = 0;
    Registro registroDados;

    // LOOP PARA PROCESSAR OS DADOS DO REGISTRO
    for(int i = 0; i < cabecalho.proxRRN; i++){

        // Fazemos a leitura do registro no arquivo (disco) e salvamos na RAM
        LerRegistroBIN(arquivoBIN, &registroDados);

        // Só iremos imprimir se não estiver logicamente removido
        // '1' == LOGICAMENTE REMOVIDO e '0' == NÃO ESTÁ MARCADO COMO REMOVIDO
        if(registroDados.removido != '1'){
            ImprimirRegistro(&registroDados);
            contadorImpressos++;
        }

        // Libera as strings alocadas pelo RegistroBIN
        LiberarStringRegistro(&registroDados);
    }

    // Se o contador de registros impressos for ZERO ao final do loop,
    // significa que todos estavam marcados como logicamente removidos
    if(contadorImpressos == 0){
        MensagemRegistroNaoEncontrado();
    }

    fclose(arquivoBIN);
}

void SelectWhere(char *arquivoEntrada, int nroBuscas){
    // Abrindo arquivo BIN para leitura binária
    FILE *arquivoBIN = fopen(arquivoEntrada, "rb");
    // Abortando funcionalidade caso ocorra erro na abertura do arquivo BIN
    if(arquivoBIN == NULL){
        MensagemErro();
        return;
    }    

    // Leitura do cabecalho do arquivo binário
    Header cabecalho;
    LerCabecalhoBIN(arquivoBIN, &cabecalho); // Para sabermos o status e o proxRRN

    if(cabecalho.status == '0'){ //arquivo inconsistente 
        MensagemErro();
        fclose(arquivoBIN);
        return;
    }    

    // Executa o número de buscas solicitadas
    for(int buscaAtual = 0; buscaAtual < nroBuscas; buscaAtual++){
        // Lê quantos filtros serão aplicados nesta busca específica
        // Quantidade de vezes que o par de critério pode repetir em uma busca
        int m_nroCriterios;
        if(scanf("%d", &m_nroCriterios) != 1) break;

        // Armazeno em uma array de struct o par de filtro que serão aplicados
        CriterioBusca criterios[m_nroCriterios];

        // Lemos os critérios de busca passados (FILTRO PASSADOS)
        LerCriteriosBusca(criterios, m_nroCriterios);

        // Posicionamos o ponteiro do arquivo para o início dos dados (IGNORANDO O CABEÇALHO = 17 Bytes)
        fseek(arquivoBIN, TAM_CABECALHO, SEEK_SET);

        Registro registroDados;
        int registroEncontrado = 0; // Flag de controle ( 0 = false, 1 = true)

        for(int rrnAtual = 0; rrnAtual < cabecalho.proxRRN; rrnAtual++){
            // "Zera" os ponteiros para prevenir lixo da memoria na leitura
            registroDados.nomeEstacao = NULL;
            registroDados.nomeLinha = NULL;

            // Preenche a struct com os dados lido do arquvio
            LerRegistroBIN(arquivoBIN, &registroDados);
            
            // Ignoramos todos os registros inválidos
            if(registroDados.removido == '0'){
                int atendeTodosCriterios = 1; // Assumimos que registro serve (true), até que se prove o contrário

                // Verifica o registro atual a partir dos criterios de busca que foram passados
                for(int criterioAtual = 0; criterioAtual < m_nroCriterios; criterioAtual++){
                    // Retornaremos 1 se o campo atende ao critério buscado
                    if(VerificaCriterioBusca(&registroDados, criterios[criterioAtual].nomeDoCampo, criterios[criterioAtual].valorBuscado) != 1){
                        atendeTodosCriterios = 0; // Falhou em dos pares de criterios (filtro) passados
                        break; // Descartamos
                    }
                }
                
                // Se passou pelas verificações, imprimimos na tela
                if(atendeTodosCriterios == 1){
                    ImprimirRegistro(&registroDados);
                    registroEncontrado = 1; // Sinalizamos que encontramos um que atende ao filtro passado
                }
            } 

            // Desaloco as strings alocadas pelo LerRegistroBIN
            LiberarStringRegistro(&registroDados);
        }

        // Passei por todo o arquivo e a flag não se alterou, continuou 0, avisa o usuário que o registro não foi encontrado
        if(registroEncontrado == 0){
            MensagemRegistroNaoEncontrado();
        }

        if(buscaAtual < nroBuscas - 1){
            printf("\n");
        }
    }

    fclose(arquivoBIN);
}

void RecuperacaoRRN(char *arquivoEntrada, int RRN){
    // Abertura do arquivo BIN para leitura
    FILE *arquivoBIN = fopen(arquivoEntrada, "rb");
    // Abortando funcionalidade caso ocorra erro na abertura do arquivo BIN
    if(arquivoBIN == NULL){
        MensagemErro();
        return;
    }

    // Leitura do cabeçalho para obter o próximo RRN disponível
    Header cabecalho;
    LerCabecalhoBIN(arquivoBIN, &cabecalho);

    // Verificação se o arquivo está consistente
    if(cabecalho.status == '0'){
        MensagemErro();
        fclose(arquivoBIN);
        return;
    }

    // Se o RRN for inválido (negativo ou além dos registros existentes), aborta
    if(RRN < 0 || RRN >= cabecalho.proxRRN){
        MensagemRegistroNaoEncontrado();
        fclose(arquivoBIN);
        return;
    }

    // Cálculo do byte offset do registro para dar fseek
    int byteoffset = TAM_CABECALHO + (RRN * TAM_REGISTRO);
    fseek(arquivoBIN, byteoffset, SEEK_SET);

    // Leitura do registro na posição calculada
    Registro registroDados;
    registroDados.nomeEstacao = NULL;
    registroDados.nomeLinha = NULL;
    LerRegistroBIN(arquivoBIN, &registroDados);

    // Se o registro estiver logicamente removido, não deve ser exibido
    if(registroDados.removido == '1'){
        MensagemRegistroNaoEncontrado();
        LiberarStringRegistro(&registroDados);
        fclose(arquivoBIN);
        return;
    }

    // Chama ImprimirRegistro para exibir os campos do registro lido
    ImprimirRegistro(&registroDados);

    LiberarStringRegistro(&registroDados);
    fclose(arquivoBIN);
}