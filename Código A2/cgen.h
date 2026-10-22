/*********************************************************/
/* Arquivo: cgen.h                                       */
/* Interface do gerador de codigo para o compilador DOMUS*/
/* Baseado em: Compiler Construction                     */
/* Kenneth C. Louden                                     */
/*********************************************************/

#ifndef _CGEN_H_
#define _CGEN_H_

/* Gera o codigo em linguagem C para o arquivo de codigo
   pela travessia da arvore sintatica */
void codeGen(TreeNode *syntaxTree, char *codefile);

#endif
