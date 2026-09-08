/******************************************************************/
/* Arquivo: main.c                                                */
/* Programa de teste para o analisador lexico do compilador DOMUS */
/******************************************************************/

#include "global.h"
#include "util.h"
#include "scan.h"

int lineno = 0;   /* Linha corrente no arquivo fonte */
FILE *source;     /* arquivo de codigo fonte */
FILE *listing;    /* arquivo texto de saida */

int EchoSource = FALSE;
int TraceScan = TRUE;
int Error = FALSE;

int main(int argc, char *argv[]) {

    TokenType token;
    const char *nomeFonte = "prog_teste.dms";

    /* o programa fonte eh lido de um arquivo externo,
       informado na linha de comando ou o padrao prog_teste.dms */
    if (argc > 1) nomeFonte = argv[1];

    printf("Abrindo programa fonte \"%s\"...\n", nomeFonte);
    if ((source = fopen(nomeFonte, "r")) == NULL) {
        perror("Abertura do arquivo fonte");
        return 1;
    }

    /* o resultado da analise eh sempre gravado no arquivo listing.txt */
    printf("Criando arquivo de saida \"listing.txt\"...\n");
    if ((listing = fopen("listing.txt", "w")) == NULL) {
        perror("Abertura de listing.txt");
        fclose(source);
        return 1;
    }

    printf("Executando analise lexica...\n");
    do {
        token = getToken();
        if (token == ERROR) Error = TRUE;
    } while (token != FIMARQ);

    printf("\nFim do arquivo alcancado. Analise lexica concluida%s.\n",
           Error ? " com erros" : " sem erros");
    printf("Resultado gravado em \"listing.txt\".\n");

    fclose(source);
    fclose(listing);
    return 0;
}