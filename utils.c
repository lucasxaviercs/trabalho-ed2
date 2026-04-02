#include "utils.h"


ControleEstacoes *InicializarControleEstacoes(){
    ControleEstacoes *controleEstacoes = malloc(sizeof(ControleEstacoes));
    controleEstacoes->capacidadeMaxima = 150;
    controleEstacoes->totalEstacoesUnicas = 0;
    controleEstacoes->listaNomesUnicos = malloc(controleEstacoes->capacidadeMaxima * sizeof(char*));
    return controleEstacoes;
}

ControlePares *InicializarControlePares(){
    ControlePares *controlePares = malloc(sizeof(ControlePares));
    controlePares->capacidadeMaxima = 150;
    controlePares->totalParesUnicos = 0;
    controlePares->listaParesUnicos = malloc(controlePares->capacidadeMaxima * sizeof(ParEstacao));
    return controlePares;
}


void RegistrarEstacaoUnica(ControleEstacoes *controleEstacoes, const char *nomeEstacao){
    if (nomeEstacao == NULL) return;

    // Verifica se o nome já não consta na lista
    for (int i = 0; i < controleEstacoes->totalEstacoesUnicas; i++){
        if (strcmp(controleEstacoes->listaNomesUnicos[i], nomeEstacao) == 0) return; 
    }

    // Aumento da capacidade (DOBRANDO) se o limite do array for atingido
    if (controleEstacoes->totalEstacoesUnicas >= controleEstacoes->capacidadeMaxima){
        controleEstacoes->capacidadeMaxima *= 2;
        controleEstacoes->listaNomesUnicos = realloc(controleEstacoes->listaNomesUnicos, controleEstacoes->capacidadeMaxima * sizeof(char*));
    }

    //Aloca o espaço do tamanho da string incuindo o '\0' para salvar a string dentro da lista
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
    if (codigoEstacaoOrigem == -1 || codigoEstacaoDestino == -1) return;

    // Verifica se a combinação desse trajeto já não está salva
    for (int i = 0; i < controlePares->totalParesUnicos; i++){
        if (controlePares->listaParesUnicos[i].codEstacaoOrigem == codigoEstacaoOrigem && controlePares->listaParesUnicos[i].codEstacaoDestino == codigoEstacaoDestino){
            return;
        }
    }

    // Dobra a capacidade se o limite do array foi atingido
    if (controlePares->totalParesUnicos >= controlePares->capacidadeMaxima){
        controlePares->capacidadeMaxima *= 2;
        controlePares->listaParesUnicos = realloc(controlePares->listaParesUnicos, controlePares->capacidadeMaxima * sizeof(ParEstacao));
    }

    //Salva a origem e destino na próxima posição livre do array
    controlePares->listaParesUnicos[controlePares->totalParesUnicos].codEstacaoOrigem = codigoEstacaoOrigem;
    controlePares->listaParesUnicos[controlePares->totalParesUnicos].codEstacaoDestino = codigoEstacaoDestino;
    
    //Incrementa o contador pares únicos, após salvar no array
    controlePares->totalParesUnicos++;
}

void MensagemErro(){
    printf("Falha no processamento do arquivo.\n");
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
void BinarioNaTela(char *arquivo) {
    FILE *fs;
    if (arquivo == NULL || !(fs = fopen(arquivo, "rb"))) {
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
    for (unsigned long i = 0; i < fl; i++) {
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
void ScanQuoteString(char *str) {
    char R;

    while ((R = getchar()) != EOF && isspace(R))
        ; // ignorar espaços, \r, \n...

    if (R == 'N' || R == 'n') { // campo NULO
        getchar();
        getchar();
        getchar();       // ignorar o "ULO" de NULO.
        strcpy(str, ""); // copia string vazia
    } else if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar();         // ignorar aspas fechando
    } else if (R != EOF) { // vc tá tentando ler uma string que não tá entre
                           // aspas! Fazer leitura normal %s então, pois deve
                           // ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    } else { // EOF
        strcpy(str, "");
    }
}