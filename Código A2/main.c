/**************************************************/
/* Arquivo: main.c                                */
/* Programa principal para o compilador DOMUS     */
/* Baseado em: Compiler Construction              */
/* Kenneth C. Louden                              */
/**************************************************/

#include "global.h"
#include "util.h"
#include "scan.h"
#include "parse.h"
#include "symtab.h"
#include "analyze.h"
#include "cgen.h"

int lineno = 0;    /* linha corrente do arquivo fonte */

FILE *source;      /* arquivo de codigo fonte                */
FILE *listing;     /* arquivo texto de saida                 */
FILE *code;        /* arquivo de codigo gerado em linguagem C*/

/* flags de rastreamento */
int EchoSource   = FALSE;
int TraceScan    = TRUE;  /* grava na listagem cada token reconhecido */
int TraceParse   = TRUE;  /* grava na listagem a arvore sintatica     */
int TraceAnalyze = TRUE;  /* grava na listagem a tabela de simbolos   */
int TraceCode    = FALSE; /* grava comentarios no codigo C gerado     */

int Error = FALSE;

int main(int argc, char *argv[]) {

    TreeNode *syntaxTree;
    char *codefile;
    int fnlen;
    const char *nomeFonte = "prog_teste2.dms";

    /* o programa fonte eh lido de um arquivo externo,
       informado na linha de comando ou o padrao prog_teste2.dms */
    if (argc > 1) nomeFonte = argv[1];

    printf("Abrindo programa fonte \"%s\"...\n", nomeFonte);
    if ((source = fopen(nomeFonte, "r")) == NULL) {
        perror("Abertura do arquivo fonte");
        return 1;
    }

    printf("Criando arquivo de saida \"listing.txt\"...\n");
    if ((listing = fopen("listing.txt", "w")) == NULL) {
        perror("Abertura de listing.txt");
        fclose(source);
        return 1;
    }

    fprintf(listing, "COMPILADOR DA LINGUAGEM DOMUS\n");
    fprintf(listing, "Programa fonte: %s\n", nomeFonte);
    if (TraceScan)
        fprintf(listing, "\nTOKENS RECONHECIDOS PELA ANALISE LEXICA:\n\n");

    /* ----------------- analise lexica e sintatica ----------------- */
    printf("Executando analise lexica e sintatica...\n");
    syntaxTree = parse();

    if (Error) {
        printf("\nEncontrado erro na analise sintatica.\n");
        printf("Resultado em \"listing.txt\".\n");
        fclose(source);
        fclose(listing);
        return 1;
    }

    if (TraceParse) {
        printf("Imprimindo arvore sintatica...\n");
        fprintf(listing, "\nARVORE SINTATICA:\n");
        printTree(syntaxTree);
    }

    /* --------------------- analise semantica ---------------------- */
    printf("Construindo a tabela de simbolos...\n");
    fprintf(listing, "\nANALISE SEMANTICA:\n");
    buildSymtab(syntaxTree);

    if (TraceAnalyze) {
        fprintf(listing, "\nTABELA DE SIMBOLOS:\n\n");
        printSymTab(listing);
        printf("\nTABELA DE SIMBOLOS:\n\n");
        printSymTab(stdout);
    }

    if (Error) {
        printf("\nAnalise semantica concluida com erros.\n");
        printf("Nenhum codigo foi gerado. Resultado em \"listing.txt\".\n");
        fclose(source);
        fclose(listing);
        return 1;
    }

    /* -------------------- geracao de codigo C --------------------- */
    fnlen = (int) strcspn(nomeFonte, "."); /* conta caracteres ate o primeiro "." */
    codefile = (char *) calloc(fnlen + 3, sizeof(char));
    strncpy(codefile, nomeFonte, fnlen);   /* copia o nome ate o primeiro "." */
    codefile[fnlen] = '\0';
    strcat(codefile, ".c");                /* acrescenta a extensao ".c" */

    printf("Gerando o codigo em linguagem C...\n");
    if ((code = fopen(codefile, "w")) == NULL) {
        printf("Nao foi possivel criar o arquivo %s\n", codefile);
        fclose(source);
        fclose(listing);
        return 1;
    }
    codeGen(syntaxTree, codefile);
    fclose(code);

    printf("\nCompilacao concluida sem erros.\n");
    printf("Codigo C gerado em \"%s\".\n", codefile);
    printf("Listagem completa em \"listing.txt\".\n");

    fclose(source);
    fclose(listing);
    return 0;
}
