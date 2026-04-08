#include "utils.h"

ControleEstacoes *InicializarControleEstacoes()
{
    ControleEstacoes *controleEstacoes = malloc(sizeof(ControleEstacoes));
    controleEstacoes->capacidadeMaxima = 150;
    controleEstacoes->totalEstacoesUnicas = 0;
    controleEstacoes->listaNomesUnicos = malloc(controleEstacoes->capacidadeMaxima * sizeof(char *));
    return controleEstacoes;
}

void RegistrarEstacaoUnica(ControleEstacoes *controleEstacoes, const char *nomeEstacao){
    if (nomeEstacao == NULL)
        return;

    // Verifica se o nome já não consta na lista
    for (int i = 0; i < controleEstacoes->totalEstacoesUnicas; i++)
    {
        if (strcmp(controleEstacoes->listaNomesUnicos[i], nomeEstacao) == 0)
            return;
    }

    // Aumento da capacidade (DOBRANDO) se o limite do array for atingido
    if (controleEstacoes->totalEstacoesUnicas >= controleEstacoes->capacidadeMaxima)
    {
        controleEstacoes->capacidadeMaxima *= 2;
        controleEstacoes->listaNomesUnicos = realloc(controleEstacoes->listaNomesUnicos, controleEstacoes->capacidadeMaxima * sizeof(char *));
    }

    // Aloca o espaço do tamanho da string incuindo o '\0' para salvar a string dentro da lista
    int tamanhoStringComNulo = strlen(nomeEstacao) + 1;
    controleEstacoes->listaNomesUnicos[controleEstacoes->totalEstacoesUnicas] = malloc(tamanhoStringComNulo * sizeof(char));
    memcpy(controleEstacoes->listaNomesUnicos[controleEstacoes->totalEstacoesUnicas], nomeEstacao, tamanhoStringComNulo);

    // Incrementamos o contador de estações únicas, após a inserção
    controleEstacoes->totalEstacoesUnicas++;
}

void LiberarControleEstacoes(ControleEstacoes *controleEstacoes){
    for (int i = 0; i < controleEstacoes->totalEstacoesUnicas; i++){
        free(controleEstacoes->listaNomesUnicos[i]);
        controleEstacoes->listaNomesUnicos[i] = NULL;
    }
    free(controleEstacoes->listaNomesUnicos);
    controleEstacoes->listaNomesUnicos = NULL;
    free(controleEstacoes);
    controleEstacoes = NULL;
}

void RegistrarParUnico(ControlePares *controlePares, int codigoEstacaoOrigem, int codigoEstacaoDestino){
    // Ignora o registro se algum dos códigos for nulo
    if (codigoEstacaoOrigem == -1 || codigoEstacaoDestino == -1)
        return;

    // Verifica se a combinação desse trajeto já não está salva
    for (int i = 0; i < controlePares->totalParesUnicos; i++)
    {
        if (controlePares->listaParesUnicos[i].codEstacaoOrigem == codigoEstacaoOrigem && controlePares->listaParesUnicos[i].codEstacaoDestino == codigoEstacaoDestino)
        {
            return;
        }
    }

    // Dobra a capacidade se o limite do array foi atingido
    if (controlePares->totalParesUnicos >= controlePares->capacidadeMaxima)
    {
        controlePares->capacidadeMaxima *= 2;
        controlePares->listaParesUnicos = realloc(controlePares->listaParesUnicos, controlePares->capacidadeMaxima * sizeof(ParEstacao));
    }

    // Salva a origem e destino na próxima posição livre do array
    controlePares->listaParesUnicos[controlePares->totalParesUnicos].codEstacaoOrigem = codigoEstacaoOrigem;
    controlePares->listaParesUnicos[controlePares->totalParesUnicos].codEstacaoDestino = codigoEstacaoDestino;

    // Incrementa o contador pares únicos, após salvar no array
    controlePares->totalParesUnicos++;
}

void LiberarControleEstacoes(ControleEstacoes *controleEstacoes){
    for (int i = 0; i < controleEstacoes->totalEstacoesUnicas; i++){
        free(controleEstacoes->listaNomesUnicos[i]);
        controleEstacoes->listaNomesUnicos[i] = NULL;
    }
    free(controleEstacoes->listaNomesUnicos);
    controleEstacoes->listaNomesUnicos = NULL;
    free(controleEstacoes);
    controleEstacoes = NULL;
}

void LiberarControlePares(ControlePares *controlePares){
    free(controlePares->listaParesUnicos);
    controlePares->listaParesUnicos = NULL;
    free(controlePares);
    controlePares = NULL;
}


void LiberarStringRegistro(Registro *registroDados){
    if (registroDados == NULL) return;

    // Libera a string e anula o ponteiro diretamente
    if (registroDados->nomeEstacao != NULL) {
        free(registroDados->nomeEstacao);
        registroDados->nomeEstacao = NULL;
    }

    if (registroDados->nomeLinha != NULL) {
        free(registroDados->nomeLinha);
        registroDados->nomeLinha = NULL;
    }
}


void LerCriteriosBusca(CriterioBusca *criterios, int qtdCriterios){
    for(int i = 0; i < qtdCriterios; i++){
        scanf("%s", criterios[i].nomeDoCampo);

        // Se for string, usamos a função fornecida para tratar as aspas
        if(strcmp(criterios[i].nomeDoCampo, "nomeEstacao") == 0 || strcmp(criterios[i].nomeDoCampo, "nomeLinha") == 0){
            ScanQuoteString(criterios[i].valorBuscado);
        }
        else{ // Se for um campo de numéricos, mantemos o scanf normal como string, mas sem utilizar o ScanQuote
            scanf("%s", criterios[i].valorBuscado);

            // A nossa função de verificação de critério, espera que NULO seja uma string vazia ""
            // Portanto, padronizados o NULO para a funcao VerificaCriterioBusca
            if(strcmp(criterios[i].valorBuscado, "NULO") == 0){
                strcpy(criterios[i].valorBuscado, "");
            }
        }
    }
}

int VerificaCriterioBusca(const Registro *registroDados, const char *nomeDoCampo, const char *valorBuscado){
    // Retornaremos 1 se o campo atende ao critério buscado, caso contrário retornaremos 0

    //A função ScanQuoteString transformará a entrada "NULO" em uma string vazia
    // Portanto, se o tamanho for 0, o usuário está buscando NULO
    int buscaPorNulo = (strlen(valorBuscado) == 0);

    // CRITÉRIO PARA CAMPOS DE TAMANHO FIXO
    if (strcmp(nomeDoCampo, "codEstacao") == 0){
        int valorInteiro = buscaPorNulo ? -1 : atoi(valorBuscado);
        return registroDados->codEstacao == valorInteiro;
    }
    else if (strcmp(nomeDoCampo, "codLinha") == 0){
        int valorInteiro = buscaPorNulo ? -1 : atoi(valorBuscado);
        return registroDados->codLinha == valorInteiro;
    }
    else if (strcmp(nomeDoCampo, "codProxEstacao") == 0){
        int valorInteiro = buscaPorNulo ? -1 : atoi(valorBuscado);
        return registroDados->codProxEstacao == valorInteiro;
    }
    else if (strcmp(nomeDoCampo, "distProxEstacao") == 0){
        int valorInteiro = buscaPorNulo ? -1 : atoi(valorBuscado);
        return registroDados->distProxEstacao == valorInteiro;
    }
    else if (strcmp(nomeDoCampo, "codLinhaIntegra") == 0){
        int valorInteiro = buscaPorNulo ? -1 : atoi(valorBuscado);
        return registroDados->codLinhaIntegra == valorInteiro;
    }
    else if (strcmp(nomeDoCampo, "codEstIntegra") == 0){
        int valorInteiro = buscaPorNulo ? -1 : atoi(valorBuscado);
        return registroDados->codEstIntegra == valorInteiro;
    }
    // CRITÉRIO PARA CAMPOS DE TAMANHO VARIÁVEL (as strings)
    else if (strcmp(nomeDoCampo, "nomeEstacao") == 0){
        // Se buscou por NULO, o registro deve estar apontando para NULL
        if (buscaPorNulo) return registroDados->nomeEstacao == NULL;

        // Se o campo do registro aponta para NULL, mas foi buscado uma palavra, então o critério não é atendido
        if (registroDados->nomeEstacao == NULL) return 0;

        // Se o campo e o valor buscado possuem dados, comparamos esse texto
        return strcmp(registroDados->nomeEstacao, valorBuscado) == 0;
    }
    else if (strcmp(nomeDoCampo, "nomeLinha") == 0){
        // Se buscou por NULO, o registro deve estar apontando para NULL
        if (buscaPorNulo) return registroDados->nomeLinha == NULL;

        // Se o campo do registro aponta para NULL, mas foi buscado uma palavra, então o critério não é atendido
        if (registroDados->nomeLinha == NULL) return 0;

        // Se o campo e o valor buscado possuem dados, comparamos esse texto
        return strcmp(registroDados->nomeLinha, valorBuscado) == 0;
    }

    // Caso algum erro aconteça, por exemplo, na passagem de argumento: algum campo de nome inválido
    return 0;
}


char VerificaEOF(FILE *arquivo){
    char ch = 0;
    ch = fgetc(arquivo); // Lê os caracteres do arquivo e retorna (-1) ou um caractere válido
    
    // Após a leitura, o ponteiro é reposicionado, ou seja, ele verifica se já é o final do arquivo
    // sem consumir o caractere válido("visualização antecipada do próximo caractere")
    fseek(arquivo, -1, SEEK_CUR);

    if (ch == EOF){
        return 0; // EOF atingido
    }else {
        return 1; // Ainda tem dados
    }
}

// Função que recebe um registro de dados e imprime seus campos no formato especificado e tratando campos nulos
void ImprimirRegistro(const Registro *registroDados){
    // Campos fixos, quando -1, imprime NULO
    // Campos variáveis, quando NULL, imprime NULO

    if (registroDados->codEstacao == -1)
        printf("NULO ");
    else
        printf("%d ", registroDados->codEstacao);

    
    if(registroDados->nomeEstacao == NULL)
        printf("NULO ");
    else
        printf("%s ", registroDados->nomeEstacao);

    if (registroDados->codLinha == -1)
        printf("NULO ");
    else
        printf("%d ", registroDados->codLinha);

    if (registroDados->nomeLinha == NULL)
        printf("NULO ");
    else
        printf("%s ", registroDados->nomeLinha);

    if (registroDados->codProxEstacao == -1)
        printf("NULO ");
    else
        printf("%d ", registroDados->codProxEstacao);

    if (registroDados->distProxEstacao == -1)
        printf("NULO ");
    else
        printf("%d ", registroDados->distProxEstacao);

    if (registroDados->codLinhaIntegra == -1)
        printf("NULO ");
    else
        printf("%d ", registroDados->codLinhaIntegra);

    // Último campo sem espaço no final e /n
    if (registroDados->codEstIntegra == -1)
        printf("NULO\n");
    else
        printf("%d\n", registroDados->codEstIntegra);
}

// MENSAGENS DE ERRO

void MensagemErro()
{
    printf("Falha no processamento do arquivo.\n");
}

void MensagemRegistroNaoEncontrado()
{
    printf("Registro inexistente.\n");
}

char VerificaEOF(FILE *arquivo){
    char ch = 0;
    ch = fgetc(arquivo); // Lê os caracteres do arquivo e retorna (-1) ou um caractere válido
    
    // Após a leitura, o ponteiro é reposicionado, ou seja, ele verifica se já é o final do arquivo
    // sem consumir o caractere válido("visualização antecipada do próximo caractere")
    fseek(arquivo, -1, SEEK_CUR);

    if (ch == EOF){
        return 0; // EOF atingido
    }else {
        return 1; // Ainda tem dados
    }
}

/*
 * Você não precisa entender o código dessa função.
 *
 * Use essa função para comparação no run.codes.
 * Lembre-se de ter fechado (fclose) o arquivo anteriormente.
 *
 * Ela vai abrir de novo para leitura e depois fechar
 * (você não vai perder pontos por isso se usar ela).
 */
void BinarioNaTela(char *arquivo)
{
    FILE *fs;
    if (arquivo == NULL || !(fs = fopen(arquivo, "rb")))
    {
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura. "
                "Ele existe e você tá passando o nome certo? Você lembrou de "
                "fechar ele com fclose depois de usar?\n");
        return;
    }

    fseek(fs, 0, SEEK_END);
    size_t fl = ftell(fs);

    fseek(fs, 0, SEEK_SET);
    unsigned char *mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    unsigned long cs = 0;
    for (unsigned long i = 0; i < fl; i++)
    {
        cs += (unsigned long)mb[i];
    }

    printf("%lf\n", (cs / (double)100));

    free(mb);
    fclose(fs);
}

/*
 *	Use essa função para ler um campo string delimitado entre aspas (").
 *	Chame ela na hora que for ler tal campo. Por exemplo:
 *
 *	A entrada está da seguinte forma:
 *		nomeDoCampo "MARIA DA SILVA"
 *
 *	Para ler isso para as strings já alocadas str1 e str2 do seu programa,
 * você faz: scanf("%s", str1); // Vai salvar nomeDoCampo em str1
 *		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2
 * (sem as aspas)
 *
 */
void ScanQuoteString(char *str)
{
    char R;

    while ((R = getchar()) != EOF && isspace(R))
        ; // ignorar espaços, \r, \n...

    if (R == 'N' || R == 'n')
    { // campo NULO
        getchar();
        getchar();
        getchar();       // ignorar o "ULO" de NULO.
        strcpy(str, ""); // copia string vazia
    }
    else if (R == '\"')
    {
        if (scanf("%[^\"]", str) != 1)
        { // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar(); // ignorar aspas fechando
    }
    else if (R != EOF)
    { // vc tá tentando ler uma string que não tá entre
      // aspas! Fazer leitura normal %s então, pois deve
      // ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    }
    else
    { // EOF
        strcpy(str, "");
    }
}