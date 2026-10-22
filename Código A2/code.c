/******************************************************/
/* Arquivo: code.c                                    */
/* Implementacao dos utilitarios de emissao de codigo */
/* para o compilador DOMUS                            */
/* Baseado em: Compiler Construction                  */
/* Kenneth C. Louden                                  */
/******************************************************/

#include "global.h"
#include "code.h"

/* Nivel de indentacao corrente do codigo gerado.
   Corresponde ao emitLoc do compilador TINY. */
static int indentLevel = 0;

/* Escreve os espacos da indentacao corrente */
void emitIndent(void) {
  int i;
  for (i = 0; i < indentLevel * LARGURA; i++)
    fprintf(code, " ");
}

/* Imprime uma linha de comentario no arquivo de codigo */
void emitComment(char *c) {
  if (TraceCode) {
    emitIndent();
    fprintf(code, "/* %s */\n", c);
  }
}

/* Escreve uma cadeia no arquivo de codigo, sem quebra de linha */
void emitString(char *s) {
  fprintf(code, "%s", s);
}

/* Escreve um numero inteiro no arquivo de codigo */
void emitNum(int n) {
  fprintf(code, "%d", n);
}

/* Quebra a linha no arquivo de codigo */
void emitNewLine(void) {
  fprintf(code, "\n");
}

/* Escreve uma linha completa: indentacao, texto e quebra de linha */
void emitLine(char *s) {
  emitIndent();
  fprintf(code, "%s\n", s);
}

/* Aumenta um nivel de indentacao */
void emitIncIndent(void) {
  indentLevel++;
}

/* Diminui um nivel de indentacao */
void emitDecIndent(void) {
  if (indentLevel > 0)
    indentLevel--;
}
