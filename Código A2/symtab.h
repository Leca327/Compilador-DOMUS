/**********************************************************/
/* Arquivo: symtab.h                                      */
/* Interface da tabela de simbolos para o compilador DOMUS*/
/* (utiliza apenas uma tabela de simbolos)                */
/* Baseado em: Compiler Construction                      */
/* Kenneth C. Louden                                      */
/**********************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

/* Lista encadeada dos numeros de linha do codigo fonte onde o
   identificador eh referenciado */
typedef struct LineListRec {
  int lineno;
  struct LineListRec *next;
} *LineList;

/* Registro das informacoes dos identificadores armazenados na
   tabela de simbolos. Alem do nome, da localizacao de memoria e
   das linhas de uso, guarda a categoria (sensor, atuador ou
   variavel), o tipo do dispositivo e a porta, que sao as
   informacoes exigidas pelas regras semanticas do Domus. */
typedef struct BucketListRec {
  char *name;
  Categoria categoria;
  TipoDispositivo tipo;
  int porta;             /* 0 quando nao se aplica            */
  int memloc;            /* Localizacao de memoria            */
  int declaracao;        /* Linha da declaracao / 1a. ocorrencia */
  LineList lines;
  struct BucketListRec *next;      /* encadeamento do hash    */
  struct BucketListRec *nextOrder; /* ordem de insercao       */
} *BucketList;

/* Insere o identificador na tabela de simbolos. Se o identificador
   ja existir, apenas acrescenta o numero da linha.
   loc = localizacao de memoria, usada somente na primeira chamada. */
void st_insert(char *name, int lineno, int loc,
               Categoria categoria, TipoDispositivo tipo, int porta);

/* Retorna a posicao do identificador na memoria ou -1 se nao encontrado */
int st_lookup(char *name);

/* Retorna o registro completo do identificador, ou NULL se nao existir */
BucketList st_bucket(char *name);

/* Retorna o registro do dispositivo que ocupa a porta, ou NULL */
BucketList st_bucket_porta(int porta);

/* Retorna o primeiro simbolo na ordem de insercao, para que o
   gerador de codigo percorra a tabela com proxOrdem */
BucketList st_first(void);

/* Mostra uma listagem formatada do conteudo da tabela de simbolos */
void printSymTab(FILE *listing);

#endif
