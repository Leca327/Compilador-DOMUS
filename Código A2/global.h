/*************************************************************/
/* Arquivo: global.h                                         */
/* Tipos globais e variaveis para o compilador DOMUS         */
/* Precisa ser chamado antes de outros arquivos de inclusao  */
/* Baseado em: Compiler Construction: Principles and Practice*/
/* Kenneth C. Louden                                         */
/*************************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* O Yacc/Bison gera internamente seus proprios valores
   para os tokens. Outros arquivos podem acessar estes
   incluindo o arquivo "tab.h" gerado pela utilizacao da
   opcao -d do Yacc/Bison (gerar cabecalho). */

#ifndef YYPARSER

/* Arquivo com definicoes de tokens gerado pelo Bison */
#include "domus.tab.h"

/* FIMARQ eh gerado implicitamente pelo Bison e nao eh
   incluido no arquivo "tab.h" */
#define FIMARQ 0

#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = quantidade maxima de palavras reservadas */
#define MAXRESERVED 13

typedef int TokenType;

extern FILE *source;  /* arquivo de codigo fonte */
extern FILE *listing; /* arquivo com texto de saida */
extern FILE *code;    /* arquivo de codigo gerado em linguagem C */

extern int lineno;    /* numero da linha do codigo fonte */
extern int Error;     /* para rastreamento de erros */

/*************************************************/
/*******  Arvore sintatica para o parser  ********/
/*************************************************/

typedef enum {StmtK, ExpK} NodeKind;
typedef enum {DeclK, IfK, WhileK, AssignK, ReadK, WriteK} StmtKind;
typedef enum {OpK, ConstK, IdK} ExpKind;

/* ExpType eh utilizado para checagem de tipo */
typedef enum {Void, Integer, Boolean} ExpType;

/* Categoria do identificador, usada nos nos de declaracao
   e, mais adiante, na tabela de simbolos */
typedef enum {CatSensor, CatAtuador, CatVariavel} Categoria;

/* Tipo do dispositivo declarado */
typedef enum {TipoAnalogico, TipoDigital, TipoInteiro} TipoDispositivo;

#define MAXCHILDREN 3

typedef struct treeNode
   { struct treeNode * child[MAXCHILDREN];
     struct treeNode * sibling;
     int lineno;
     NodeKind nodekind;
     union { StmtKind stmt; ExpKind exp; } kind;
     union { TokenType op;
             int val;
             char * name; } attr;
     ExpType type;             /* para checagem de tipo das expressoes */
     Categoria categoria;      /* sensor, atuador ou variavel          */
     TipoDispositivo tipoDisp; /* analogico ou digital                 */
     int porta;                /* porta do sensor/atuador              */
   } TreeNode;

/*****************************************************/
/***********   Flags para rastreamento    ************/
/*****************************************************/

/* EchoSource = TRUE faz o programa fonte ser impresso no
   arquivo listing com o numero de linha durante a analise
   sintatica */
extern int EchoSource;

/* TraceScan = TRUE faz a informacao do token ser impressa
   quando cada token eh reconhecido pela varredura */
extern int TraceScan;

/* TraceParse = TRUE faz a arvore sintatica ser impressa
   de forma linearizada (identada por filho) */
extern int TraceParse;

/* TraceAnalyze = TRUE faz a tabela de simbolos ser mostrada
   no arquivo listing */
extern int TraceAnalyze;

/* TraceCode = TRUE faz os comentarios serem gravados no
   arquivo de codigo em linguagem C quando o codigo eh gerado */
extern int TraceCode;

#endif
