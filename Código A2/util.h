/****************************************************/
/* Arquivo: util.h                                  */
/* Utilitarios e funcoes para o compilador DOMUS    */
/****************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

/* Imprime o token e seu lexema */
void printToken( TokenType, const char* );

/* Cria um no de comando para construcao da arvore sintatica */
TreeNode * newStmtNode(StmtKind);

/* Cria um no de expressao para construcao da arvore sintatica */
TreeNode * newExpNode(ExpKind);

/* Aloca espaco e faz copia de uma string */
char * copyString( char * );

/* Imprime a arvore sintatica usando indentacao para indicar as subarvores */
void printTree( TreeNode * );

/* Descricoes textuais usadas na impressao da arvore */
const char * nomeCategoria( Categoria );
const char * nomeTipoDisp( TipoDispositivo );

#endif
