/*****************************************************************/
/* Arquivo: util.c                                               */
/* Implementacao de funcoes e utilitarios para o compilador DOMUS*/
/*****************************************************************/

#include "global.h"
#include "util.h"

/* Imprime um token e seu lexema no arquivo listing */
void printToken( TokenType token, const char* tokenString ) {
  switch (token) {
    case SENSOR:
    case ATUADOR:
    case PORTA:
    case ANALOGICO:
    case DIGITAL:
    case SE:
    case ENTAO:
    case SENAO:
    case FIM_SE:
    case ENQUANTO:
    case FIM_ENQUANTO:
    case LER:
    case ESCREVER:
      fprintf(listing, "palavra reservada: %s\n", tokenString);
      break;
    case ATRIB: fprintf(listing, "ATRIBUICAO: <-\n"); break;
    case IG:     fprintf(listing, "IGUAL: ==\n"); break;
    case DIF:     fprintf(listing, "DIFERENTE: !=\n"); break;
    case ME:     fprintf(listing, "MENOR: <\n"); break;
    case MEQ:     fprintf(listing, "MENOR_IGUAL: <=\n"); break;
    case MA:     fprintf(listing, "MAIOR: >\n"); break;
    case MAQ:     fprintf(listing, "MAIOR_IGUAL: >=\n"); break;
    case MAIS:   fprintf(listing, "SOMA: +\n"); break;
    case MENOS:  fprintf(listing, "SUBTRACAO: -\n"); break;
    case VEZES:  fprintf(listing, "MULTIPLICACAO: *\n"); break;
    case DIV:   fprintf(listing, "DIVISAO: /\n"); break;
    case EPAREN: fprintf(listing, "ABRE_PARENTESES: (\n"); break;
    case DPAREN: fprintf(listing, "FECHA_PARENTESES: )\n"); break;
    case PONT:    fprintf(listing, "SEPARADOR_COMANDO: .\n"); break;
    case DPONT:  fprintf(listing, "SEPARADOR_TIPO: :\n"); break;
    case VIRG:  fprintf(listing, "SEPARADOR_DECLARACAO: ,\n"); break;
    case FIMARQ: fprintf(listing, "EOF (fim de arquivo)\n"); break;
    case NUM:
      fprintf(listing, "NUM, val= %s\n", tokenString);
      break;
    case ID:
      fprintf(listing, "ID, nome= %s\n", tokenString);
      break;
    case ERROR:
      fprintf(listing, "ERRO: %s\n", tokenString);
      break;
    default: /* nao deve acontecer nunca */
      fprintf(listing, "Token desconhecido: %d\n", token);
  }
}
