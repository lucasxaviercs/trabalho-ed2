#include "arquivosLerEscrever.h"

#define SEPARADOR_CAMPOS ','


static int LerCampoFixo(char **linha){
    char *campo = strsep(linha, SEPARADOR_CAMPOS);

    // Os valores de campos nulos devem ser representados pelo valor -1
    if(campo == NULL || campo[0] == '\0') return -1;

    return (int)strtol(campo, NULL, 10);
}


static char *LerCampoVariavel(char **linha){
    char *campo = strsep(linha, SEPARADOR_CAMPOS);

    if(campo == NULL || campo[0] == '\0'){
        return NULL;
    }

    char *nome = malloc( (strlen(campo) + 1) * sizeof(char) );
    strcpy(nome, campo);
    return nome;
}

static int EscreverStringVariavelBIN(FILE *arquivoBIN, int tamanho, const char *string){
    int espacoUtilizado = 0;
    
    // Escreve o indicador de tamanho (4 bytes) primeiro
    fwrite(&tamanho, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);
    
    // Se o tamanho for maior que 0, escreve a string (sem o '\0')
    if (tamanho > 0 && string != NULL) {
        fwrite(string, sizeof(char), tamanho, arquivoBIN);
        espacoUtilizado += tamanho;
    }
    
    return espacoUtilizado;
}

static void PreencherComLixoBIN(FILE *arquivoBIN, int espacoUtilizado, int tamanhoTotal){
    char lixo = '$';
    int bytesRestantes = tamanhoTotal - espacoUtilizado;
    
    for (int i = 0; i < bytesRestantes; i++){
        fwrite(&lixo, sizeof(char), 1, arquivoBIN);
    }
}

void IgnorarLinhaZeroCSV(FILE *arquivoCSV){
    char *buffer =  malloc(256 * sizeof(char));
    if(buffer == NULL){
        printf("Erro na alocação de memória!\n");
        exit(1);
    }

    if(fgets(buffer, 256, arquivoCSV) == NULL){
        // MENSAGEM EXIGIDA quando houver falha no processamento de algum arquivo
        MensagemErro();
        free(buffer);
        buffer = NULL;
        exit(1);
    }
    
    free(buffer);
    buffer = NULL;
}


Header InicializarCabecalho(){
    Header cabecalho;

    // Inicialização do cabeçalho na memória primária (RAM)
    cabecalho.status = '0'; // Para quando abrir para escrita estar inconsistente
    cabecalho.topo = -1; // Não há registros logicamente removidos
    cabecalho.proxRRN = 0; // O próximo RRN disponível deve ser iniciado com o valor 0
    cabecalho.nroEstacoes = 0; // Indica a quantidade de estações
    cabecalho.nroParesEstacao = 0; // Indica a quantidade de pares

    return cabecalho;
}

Header LerCabecalhoBIN(FILE *arquivoBIN, const Header cabecalho){
    fseek(arquivoBIN, 0, SEEK_SET);

    fread(&cabecalho.status, sizeof(char), 1, arquivoBIN);
    fread(&cabecalho.topo, sizeof(int), 1, arquivoBIN);
    fread(&cabecalho.proxRRN, sizeof(int), 1, arquivoBIN);
    fread(&cabecalho.nroEstacoes, sizeof(int), 1, arquivoBIN);
    fread(&cabecalho.nroParesEstacao, sizeof(int), 1, arquivoBIN);

    return cabecalho;
}

Registro LerRegistroBIN(FILE *arquivoBIN, Registro registroDados){
    // Leitura dos campos de tamanho fixo
    fread(&registroDados.removido, sizeof(char), 1, arquivoBIN);
    fread(&registroDados.proximo, sizeof(int), 1, arquivoBIN);
    fread(&registroDados.codEstacao, sizeof(int), 1, arquivoBIN);
    fread(&registroDados.codLinha, sizeof(int), 1, arquivoBIN);
    fread(&registroDados.codProxEstacao, sizeof(int), 1, arquivoBIN);
    fread(&registroDados.distProxEstacao, sizeof(int), 1, arquivoBIN);
    fread(&registroDados.codLinhaIntegra, sizeof(int), 1, arquivoBIN);
    fread(&registroDados.codEstIntegra, sizeof(int), 1, arquivoBIN);

    // Leitura dos campos de tamanho variável:
    // primeiro lê o int que indica o tamanho, depois aloca e lê a string
    fread(&registroDados.tamNomeEstacao, sizeof(int), 1, arquivoBIN);
    if(registroDados.tamNomeEstacao > 0){
        registroDados.nomeEstacao = malloc((registroDados.tamNomeEstacao + 1) * sizeof(char));
        fread(registroDados.nomeEstacao, sizeof(char), registroDados.tamNomeEstacao, arquivoBIN);
        registroDados.nomeEstacao[registroDados.tamNomeEstacao] = '\0';
    } else {
        registroDados.nomeEstacao = NULL;
    }

    fread(&registroDados.tamNomeLinha, sizeof(int), 1, arquivoBIN);
    if(registroDados.tamNomeLinha > 0){
        registroDados.nomeLinha = malloc((registroDados.tamNomeLinha + 1) * sizeof(char));
        fread(registroDados.nomeLinha, sizeof(char), registroDados.tamNomeLinha, arquivoBIN);
        registroDados.nomeLinha[registroDados.tamNomeLinha] = '\0';
    } else {
        registroDados.nomeLinha = NULL;
    }

    return registroDados;
}

Registro LerRegistroCSV(FILE *arquivoCSV, Registro registroDados){
    char *buffer = malloc(512 * sizeof(char));
    if(buffer == NULL){
        printf("Erro na alocação de memória!\n");
    }

    if(fgets(buffer, 512, arquivoCSV) == NULL){
        registroDados.removido = '1';
        free(buffer);
        buffer = NULL;
        return registroDados;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    char *linha = buffer;

    registroDados.removido = '0';
    registroDados.proximo = -1;
    registroDados.codEstacao = LerCampoFixo(&linha);
    registroDados.codLinha = LerCampoFixo(&linha);
    registroDados.codProxEstacao = LerCampoFixo(&linha);
    registroDados.distProxEstacao = LerCampoFixo(&linha);
    registroDados.codLinhaIntegra = LerCampoFixo(&linha);
    registroDados.codEstIntegra = LerCampoFixo(&linha);
    registroDados.nomeEstacao = LerCampoVariavel(&linha);
    registroDados.tamNomeEstacao = (registroDados.nomeEstacao == NULL) ? 0 : strlen(registroDados.nomeEstacao);
    registroDados.nomeLinha = LerCampoVariavel(&linha);
    registroDados.tamNomeLinha = (registroDados.nomeLinha == NULL) ? 0 : strlen(registroDados.nomeLinha);

    free(buffer);
    buffer = NULL;

    return registroDados;
}

void EscreverCabecalhoBIN(FILE* arquivoBIN, const Header* cabecalho){
    fseek(arquivoBIN, 0, SEEK_SET);

    fwrite(&cabecalho->status, sizeof(char), 1, arquivoBIN);
    fwrite(&cabecalho->topo, sizeof(int), 1, arquivoBIN);
    fwrite(&cabecalho->proxRRN, sizeof(int), 1, arquivoBIN);
    fwrite(&cabecalho->nroEstacoes, sizeof(int), 1, arquivoBIN);
    fwrite(&cabecalho->nroParesEstacao, sizeof(int), 1, arquivoBIN);
}

void EscreverRegistroBIN(FILE *arquivoBIN, const Registro *registroDados) {
    int espacoUtilizado = 0;

    // ESCREVE SEGUINDO A ORDEM DOS CAMPOS NO REGISTRO
    // Escreve os campos de tamanho fixo
    fwrite(&registroDados->removido, sizeof(char), 1, arquivoBIN);
    espacoUtilizado += sizeof(char);

    fwrite(&registroDados->proximo, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);

    fwrite(&registroDados->codEstacao, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);

    fwrite(&registroDados->codLinha, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);

    fwrite(&registroDados->codProxEstacao, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);

    fwrite(&registroDados->distProxEstacao, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);

    fwrite(&registroDados->codLinhaIntegra, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);

    fwrite(&registroDados->codEstIntegra, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);

    // Escrever os campos de tamanho variável escrevendo antes em um campo de 4 bytes o tamanho do campo.
    espacoUtilizado += EscreverStringVariavelBIN(arquivoBIN, registroDados->tamNomeEstacao, registroDados->nomeEstacao);
    espacoUtilizado += EscreverStringVariavelBIN(arquivoBIN, registroDados->tamNomeLinha, registroDados->nomeLinha);

    // Preenche o bytes não ocupados com lixo ('$')
    PreencherComLixoBIN(arquivoBIN, espacoUtilizado, TAM_REGISTRO);
}

void LiberarStringRegistro(Registro *registroDados){
    if(registroDados->nomeEstacao != NULL){
        free(registroDados->nomeEstacao);
        registroDados->nomeEstacao = NULL;
    }

    if(registroDados->nomeLinha != NULL){
        free(registroDados->nomeLinha);
        registroDados->nomeLinha = NULL;
    }

}