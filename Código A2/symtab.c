/**********************************************************************/
/* Arquivo: symtab.c                                                  */
/* Implementacao da tabela de simbolos para o compilador DOMUS        */
/* (utiliza apenas uma tabela de simbolos)                            */
/* A tabela de simbolos eh implementada como uma tabela hash encadeada*/
/* Baseado em: Compiler Construction                                  */
/* Kenneth C. Louden                                                  */
/**********************************************************************/

#include "global.h"
#include "util.h"
#include "symtab.h"

/* SIZE -> Tamanho da tabela hash */
#define SIZE 211

/* SHIFT -> Valor usado como multiplicador para o calculo do hash */
#define SHIFT 4

/* Funcao de hash */
static int hash(char *key) {
  int temp = 0;
  int i = 0;
  while (key[i] != '\0') {
    temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/* Tabela hash */
static BucketList hashTable[SIZE];

/* Lista auxiliar que guarda os simbolos na ordem em que foram
   inseridos, para que a tabela seja sempre impressa na mesma
   ordem em que os identificadores aparecem no programa fonte */
static BucketList listaOrdem = NULL;
static BucketList ultimoOrdem = NULL;

BucketList st_bucket(char *name) {
  int h = hash(name);
  BucketList l = hashTable[h];
  while ((l != NULL) && (strcmp(name, l->name) != 0))
    l = l->next;
  return l;
}

BucketList st_bucket_porta(int porta) {
  BucketList l;
  if (porta <= 0) return NULL;
  for (l = listaOrdem; l != NULL; l = l->nextOrder)
    if (l->porta == porta) return l;
  return NULL;
}

/* Insere o identificador na tabela de simbolos. Se ja existir,
   apenas acrescenta o numero da linha de uso. */
void st_insert(char *name, int lineno, int loc,
               Categoria categoria, TipoDispositivo tipo, int porta) {
  int h = hash(name);
  BucketList l = hashTable[h];
  while ((l != NULL) && (strcmp(name, l->name) != 0))
    l = l->next;
  if (l == NULL) { /* Identificador ainda nao esta na tabela de simbolos */
    l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->categoria = categoria;
    l->tipo = tipo;
    l->porta = porta;
    l->memloc = loc;
    l->declaracao = lineno;
    l->lines = NULL;
    l->next = hashTable[h];
    hashTable[h] = l;
    /* encadeia tambem na ordem de insercao */
    l->nextOrder = NULL;
    if (listaOrdem == NULL) listaOrdem = l;
    else ultimoOrdem->nextOrder = l;
    ultimoOrdem = l;
  }
  else { /* Identificador encontrado. Apenas acrescenta o numero da linha. */
    LineList t = l->lines;
    LineList novo;
    while ((t != NULL) && (t->lineno != lineno) && (t->next != NULL))
      t = t->next;
    if ((t != NULL) && (t->lineno == lineno)) return; /* linha ja registrada */
    novo = (LineList) malloc(sizeof(struct LineListRec));
    novo->lineno = lineno;
    novo->next = NULL;
    if (t == NULL) l->lines = novo;
    else t->next = novo;
  }
} /* st_insert */

/* Retorna o primeiro simbolo na ordem de insercao */
BucketList st_first(void) {
  return listaOrdem;
}

/* Retorna a posicao do identificador na memoria ou -1 se nao encontrado */
int st_lookup(char *name) {
  BucketList l = st_bucket(name);
  if (l == NULL)
    return -1;
  else
    return l->memloc;
}

/* Mostra uma listagem formatada do conteudo da tabela de simbolos */
void printSymTab(FILE *listing) {
  BucketList l;
  LineList t;
  fprintf(listing, "Nome             Categoria  Tipo       Porta  Local  Decl.  Linhas de uso\n");
  fprintf(listing, "---------------- ---------- ---------- -----  -----  -----  -------------\n");
  for (l = listaOrdem; l != NULL; l = l->nextOrder) {
    fprintf(listing, "%-16s ", l->name);
    fprintf(listing, "%-10s ", nomeCategoria(l->categoria));
    fprintf(listing, "%-10s ", nomeTipoDisp(l->tipo));
    if (l->porta > 0)
      fprintf(listing, "%-5d  ", l->porta);
    else
      fprintf(listing, "%-5s  ", "-");
    fprintf(listing, "%-5d  ", l->memloc);
    fprintf(listing, "%-5d  ", l->declaracao);
    for (t = l->lines; t != NULL; t = t->next)
      fprintf(listing, "%d ", t->lineno);
    fprintf(listing, "\n");
  }
} /* printSymTab */
