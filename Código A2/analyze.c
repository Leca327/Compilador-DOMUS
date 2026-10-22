/****************************************************************/
/* Arquivo: analyze.c                                           */
/* Implementacao do analisador semantico para o compilador DOMUS*/
/* Baseado em: Compiler Construction                            */
/* Kenneth C. Louden                                            */
/*                                                              */
/* Regras semanticas verificadas (Regras da linguagem Domus):   */
/*  1. sensores e atuadores nao podem ser declarados duas vezes;*/
/*  2. cada sensor/atuador deve ocupar uma porta exclusiva;     */
/*  3. a variavel eh criada na primeira atribuicao ou em ler(); */
/*  4. usar uma variavel ainda nao definida eh erro;            */
/*  5. sensor eh de somente leitura (gravar nele eh erro);      */
/*  6. atuador eh de somente escrita (ler dele eh erro).        */
/****************************************************************/

#include "global.h"
#include "util.h"
#include "symtab.h"
#include "analyze.h"

/* Conta a localizacao do identificador na memoria */
static int location = 0;

/* Exibe mensagem de erro semantico */
static void semanticError(TreeNode *t, char *message) {
  fprintf(listing, ">>> Erro semantico (linha %d): %s\n", t->lineno, message);
  printf("Erro semantico (linha %d): %s\n", t->lineno, message);
  Error = TRUE;
}

/* Regras 1 e 2: declaracao de um sensor ou de um atuador */
static void declaraDispositivo(TreeNode *t) {
  char msg[256];
  BucketList l = st_bucket(t->attr.name);
  BucketList dono;

  if (l != NULL) { /* Regra 1 */
    sprintf(msg, "o identificador \"%s\" ja foi declarado como %s na linha %d",
            t->attr.name, nomeCategoria(l->categoria), l->declaracao);
    semanticError(t, msg);
    return;
  }

  dono = st_bucket_porta(t->porta); /* Regra 2 */
  if (dono != NULL) {
    sprintf(msg, "a porta %d ja esta alocada para o %s \"%s\" (linha %d)",
            t->porta, nomeCategoria(dono->categoria), dono->name, dono->declaracao);
    semanticError(t, msg);
  }

  /* o dispositivo eh inserido mesmo apos erro de porta, para que os
     usos seguintes nao gerem erros encadeados */
  st_insert(t->attr.name, t->lineno, location++,
            t->categoria, t->tipoDisp, t->porta);
}

/* Regras 3 e 5: gravacao em um identificador
   (lado esquerdo de uma atribuicao ou argumento de ler) */
static void gravaIdentificador(TreeNode *t) {
  char msg[256];
  BucketList l = st_bucket(t->attr.name);

  if (l == NULL) { /* Regra 3: variavel criada na primeira atribuicao */
    st_insert(t->attr.name, t->lineno, location++,
              CatVariavel, TipoInteiro, 0);
    return;
  }

  st_insert(t->attr.name, t->lineno, 0, CatVariavel, TipoInteiro, 0);

  if (l->categoria == CatSensor) { /* Regra 5 */
    sprintf(msg, "tentativa de gravacao no sensor \"%s\"; sensores sao de somente leitura",
            t->attr.name);
    semanticError(t, msg);
  }
}

/* Regras 4 e 6: leitura de um identificador dentro de uma expressao */
static void leIdentificador(TreeNode *t) {
  char msg[256];
  BucketList l = st_bucket(t->attr.name);

  if (l == NULL) { /* Regra 4 */
    sprintf(msg, "a variavel \"%s\" foi utilizada sem ter sido definida",
            t->attr.name);
    semanticError(t, msg);
    return;
  }

  st_insert(t->attr.name, t->lineno, 0, CatVariavel, TipoInteiro, 0);

  if (l->categoria == CatAtuador) { /* Regra 6 */
    sprintf(msg, "tentativa de leitura do atuador \"%s\"; atuadores sao de somente escrita",
            t->attr.name);
    semanticError(t, msg);
  }
}

/* Insere os identificadores armazenados em t na tabela de simbolos */
static void insertNode(TreeNode *t) {
  switch (t->nodekind) {
    case StmtK: /* Se for um comando */
      switch (t->kind.stmt) {
        case DeclK: /* Declaracao de sensor ou atuador */
          declaraDispositivo(t);
          break;
        case AssignK: /* Atribuicao: grava no identificador */
        case ReadK:   /* Leitura de dados: grava no identificador */
          gravaIdentificador(t);
          break;
        default:
          break;
      }
      break;
    case ExpK: /* Se for uma expressao */
      switch (t->kind.exp) {
        case IdK: /* Identificador usado em expressao: leitura */
          leIdentificador(t);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

/* Constroi a tabela de simbolos varrendo a arvore sintatica em pre-ordem.
   A atribuicao eh o unico caso tratado fora da pre-ordem: a expressao do
   lado direito precisa ser analisada antes do identificador do lado
   esquerdo, para que "x <- x + 1" com x ainda indefinido seja acusado. */
void buildSymtab(TreeNode *t) {
  if (t != NULL) {
    int i;
    if ((t->nodekind == StmtK) && (t->kind.stmt == AssignK)) {
      buildSymtab(t->child[0]);
      insertNode(t);
    }
    else {
      insertNode(t);
      for (i = 0; i < MAXCHILDREN; i++)
        buildSymtab(t->child[i]);
    }
    buildSymtab(t->sibling);
  }
}
