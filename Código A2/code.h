/******************************************************/
/* Arquivo: code.h                                    */
/* Interface para os utilitarios de emissao de codigo */
/* para o compilador DOMUS                            */
/* Baseado em: Compiler Construction                  */
/* Kenneth C. Louden                                  */
/*                                                    */
/* No compilador TINY estes utilitarios emitem        */
/* instrucoes da maquina TM. Como o alvo do Domus eh  */
/* a linguagem C, eles emitem texto e controlam a     */
/* indentacao do programa gerado.                     */
/******************************************************/

#ifndef _CODE_H_
#define _CODE_H_

/* LARGURA = quantidade de espacos de cada nivel de indentacao */
#define LARGURA 4

/* Imprime uma linha de comentario no arquivo de codigo */
void emitComment(char *c);

/* Escreve os espacos da indentacao corrente */
void emitIndent(void);

/* Escreve uma cadeia no arquivo de codigo, sem quebra de linha */
void emitString(char *s);

/* Escreve um numero inteiro no arquivo de codigo */
void emitNum(int n);

/* Quebra a linha no arquivo de codigo */
void emitNewLine(void);

/* Escreve uma linha completa: indentacao, texto e quebra de linha */
void emitLine(char *s);

/* Aumenta e diminui um nivel de indentacao */
void emitIncIndent(void);
void emitDecIndent(void);

#endif
