// FUNÇÕES AUXILIARES PARA LEITURA E ESCRITA DE REGISTROS
#include "arquivosLerEscrever.h"

#define SEPARADOR_CAMPOS ","

// ----- LEITURA DO CSV -----

// Essa função ignora a primeira linha do csv que mostra os nomes dos campos, de forma que a leitura comece a partir dos dados de fato
void IgnorarLinhaZeroCSV(FILE *arquivoCSV){
    char buffer[256];
    if(fgets(buffer, 256, arquivoCSV) == NULL){
        // MENSAGEM EXIGIDA quando houver falha no processamento de algum arquivo
        MensagemErro();
        exit(1);
    }
}

static int LerCampoFixo(char **linha){
    char *campo = strsep(linha, SEPARADOR_CAMPOS);

    // Os valores de campos nulos devem ser representados pelo valor -1
    if(campo == NULL || campo[0] == '\0' || strcmp(campo, "NULO") == 0) return -1;

    return (int)strtol(campo, NULL, 10);
}


static char *LerCampoVariavel(char **linha){
    char *campo = strsep(linha, SEPARADOR_CAMPOS);

    if(campo == NULL || campo[0] == '\0' || strcmp(campo, "NULO") == 0) return NULL;

    char *nome = malloc( (strlen(campo) + 1) * sizeof(char) );
    strcpy(nome, campo);
    return nome;
}

// Essa função lê o arquivo CSV e preenche uma struct Registro com os dados lidos
void LerRegistroCSV(FILE *arquivoCSV, Registro *registroDados){
    char buffer[512];
    if(fgets(buffer, 512, arquivoCSV) == NULL){
        registroDados->removido = '1';
        return;
    }
    buffer[strcspn(buffer, "\r\n")] = '\0';

    // PREVENÇÃO
    if(strlen(buffer) == 0){
        registroDados->removido = '1';
        return;
    }

    char *linha = buffer;

    registroDados->removido = '0';
    registroDados->proximo = -1;

    registroDados->codEstacao = LerCampoFixo(&linha);
    registroDados->nomeEstacao = LerCampoVariavel(&linha);
    registroDados->tamNomeEstacao = (registroDados->nomeEstacao == NULL) ? 0 : strlen(registroDados->nomeEstacao);

    registroDados->codLinha = LerCampoFixo(&linha);
    registroDados->nomeLinha = LerCampoVariavel(&linha);
    registroDados->tamNomeLinha = (registroDados->nomeLinha == NULL) ? 0 : strlen(registroDados->nomeLinha);

    registroDados->codProxEstacao = LerCampoFixo(&linha);
    registroDados->distProxEstacao = LerCampoFixo(&linha);
    registroDados->codLinhaIntegra = LerCampoFixo(&linha);
    registroDados->codEstIntegra = LerCampoFixo(&linha);
}


// ----- LEITURA DO ARQUIVO BINÁRIO -----

// As funções LerCabecalhoBIN e LerRegistroBIN leem o cabecalho e os registros de um arquivo BIN e preenchem as structs
void LerCabecalhoBIN(FILE *arquivoBIN, Header *cabecalho){
    if(cabecalho == NULL) return;

    // Garante estar no início do arquivo
    fseek(arquivoBIN, 0, SEEK_SET);

    fread(&cabecalho->status, sizeof(char), 1, arquivoBIN);
    fread(&cabecalho->topo, sizeof(int), 1, arquivoBIN);
    fread(&cabecalho->proxRRN, sizeof(int), 1, arquivoBIN);
    fread(&cabecalho->nroEstacoes, sizeof(int), 1, arquivoBIN);
    fread(&cabecalho->nroParesEstacao, sizeof(int), 1, arquivoBIN);
}

void LerRegistroBIN(FILE *arquivoBIN, Registro *registroDados){
    // Para não ficar lendo LIXO '$'
    int espacoUtilizado = 0;
   
    // Leitura dos campos de tamanho fixo
    fread(&registroDados->removido, sizeof(char), 1, arquivoBIN);
    espacoUtilizado += sizeof(char);
    fread(&registroDados->proximo, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);
    fread(&registroDados->codEstacao, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);
    fread(&registroDados->codLinha, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);
    fread(&registroDados->codProxEstacao, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);
    fread(&registroDados->distProxEstacao, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);
    fread(&registroDados->codLinhaIntegra, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);
    fread(&registroDados->codEstIntegra, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);

    // Leitura dos campos de tamanho variável:
    // primeiro lê o int que indica o tamanho, depois aloca e lê a string
    fread(&registroDados->tamNomeEstacao, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);
    if(registroDados->tamNomeEstacao > 0){
        registroDados->nomeEstacao = malloc((registroDados->tamNomeEstacao + 1) * sizeof(char));
        fread(registroDados->nomeEstacao, sizeof(char), registroDados->tamNomeEstacao, arquivoBIN);
        registroDados->nomeEstacao[registroDados->tamNomeEstacao] = '\0';
        espacoUtilizado += registroDados->tamNomeEstacao;
    } else {
        registroDados->nomeEstacao = NULL;
    }

    fread(&registroDados->tamNomeLinha, sizeof(int), 1, arquivoBIN);
    espacoUtilizado += sizeof(int);
    if(registroDados->tamNomeLinha > 0){
        registroDados->nomeLinha = malloc((registroDados->tamNomeLinha + 1) * sizeof(char));
        fread(registroDados->nomeLinha, sizeof(char), registroDados->tamNomeLinha, arquivoBIN);
        registroDados->nomeLinha[registroDados->tamNomeLinha] = '\0';
        espacoUtilizado += registroDados->tamNomeLinha;
    } else {
        registroDados->nomeLinha = NULL;
    }

    // PULAR OS '$' NA LEITURA DOS 80 BYTES
    int bytesRestantes = TAM_REGISTRO - espacoUtilizado;
    if(bytesRestantes > 0){
        fseek(arquivoBIN, bytesRestantes, SEEK_CUR);
    }
}

// ESCRITA NO ARQUIVO BINÁRIO

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

// As funções EscreverCabecalhoBIN e EscreverRegistroBIN escrevem o cabecalho e os registros em um arquivo BIN
void EscreverCabecalhoBIN(FILE* arquivoBIN, const Header* cabecalho){
    // Garante estar no início do arquivo
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

// ----- OUTRAS FUNÇÕES AUXILIARES -----

// A função InicializarCabecalho aloca memória e seta os campos com valores iniciais
Header *InicializarCabecalho(){
    Header *cabecalho = malloc(sizeof(Header));

    if (cabecalho != NULL){
        // Inicialização do cabeçalho na memória primária (RAM)
        cabecalho->status = '0'; // Para quando abrir para escrita estar inconsistente
        cabecalho->topo = -1; // Não há registros logicamente removidos
        cabecalho->proxRRN = 0; // O próximo RRN disponível deve ser iniciado com o valor 0
        cabecalho->nroEstacoes = 0; // Indica a quantidade de estações
        cabecalho->nroParesEstacao = 0; // Indica a quantidade de pares
    }
    
    return cabecalho;
}




