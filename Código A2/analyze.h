/************************************************************/
/* Arquivo: analyze.h                                       */
/* Interface do analisador semantico para o compilador DOMUS*/
/* Baseado em: Compiler Construction                        */
/* Kenneth C. Louden                                        */
/************************************************************/

#ifndef _ANALYZE_H_
#define _ANALYZE_H_

/* Constroi a tabela de simbolos varrendo a arvore sintatica e
   aplica as regras semanticas da linguagem Domus */
void buildSymtab(TreeNode *);

#endif
