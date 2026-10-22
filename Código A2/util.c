/*****************************************************************/
/* Arquivo: util.c                                               */
/* Implementacao de funcoes e utilitarios para o compilador DOMUS*/
/* Baseado em: Compiler Construction                             */
/* Kenneth C. Louden                                             */
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
    case ATRIB:  fprintf(listing, "ATRIBUICAO: <-\n"); break;
    case IG:     fprintf(listing, "IGUAL: ==\n"); break;
    case DIF:    fprintf(listing, "DIFERENTE: !=\n"); break;
    case ME:     fprintf(listing, "MENOR: <\n"); break;
    case MEQ:    fprintf(listing, "MENOR_IGUAL: <=\n"); break;
    case MA:     fprintf(listing, "MAIOR: >\n"); break;
    case MAQ:    fprintf(listing, "MAIOR_IGUAL: >=\n"); break;
    case MAIS:   fprintf(listing, "SOMA: +\n"); break;
    case MENOS:  fprintf(listing, "SUBTRACAO: -\n"); break;
    case VEZES:  fprintf(listing, "MULTIPLICACAO: *\n"); break;
    case DIV:    fprintf(listing, "DIVISAO: /\n"); break;
    case EPAREN: fprintf(listing, "ABRE_PARENTESES: (\n"); break;
    case DPAREN: fprintf(listing, "FECHA_PARENTESES: )\n"); break;
    case PONT:   fprintf(listing, "SEPARADOR_COMANDO: .\n"); break;
    case DPONT:  fprintf(listing, "SEPARADOR_TIPO: :\n"); break;
    case VIRG:   fprintf(listing, "SEPARADOR_DECLARACAO: ,\n"); break;
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

/* Cria um no de comando para a construcao da arvore sintatica */
TreeNode * newStmtNode(StmtKind kind) {
  TreeNode *t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Falta de memoria na linha %d\n", lineno);
  else {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
    t->categoria = CatVariavel;
    t->tipoDisp = TipoInteiro;
    t->porta = 0;
  }
  return t;
}

/* Cria um no de expressao para a construcao da arvore sintatica */
TreeNode * newExpNode(ExpKind kind) {
  TreeNode *t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Falta de memoria na linha %d\n", lineno);
  else {
    for (i = 0; i < MAXCHILDREN; i++)
      t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
    t->categoria = CatVariavel;
    t->tipoDisp = TipoInteiro;
    t->porta = 0;
  }
  return t;
}

/* Aloca espaco e faz copia de uma string existente */
char * copyString(char * s) {
  int n;
  char * t;
  if (s == NULL)
    return NULL;
  n = strlen(s) + 1;
  t = malloc(n);
  if (t == NULL)
    fprintf(listing, "Falta de memoria na linha %d\n", lineno);
  else
    strcpy(t, s);
  return t;
}

/* Descricoes textuais usadas na impressao da arvore */
const char * nomeCategoria( Categoria c ) {
  switch (c) {
    case CatSensor:  return "sensor";
    case CatAtuador: return "atuador";
    default:         return "variavel";
  }
}

const char * nomeTipoDisp( TipoDispositivo t ) {
  switch (t) {
    case TipoAnalogico: return "analogico";
    case TipoDigital:   return "digital";
    default:            return "inteiro";
  }
}

/* A variavel indentno eh usada por printTree para armazenar a
   quantidade de espacos a indentar */
static int indentno = 0;

/* Macros para incrementar e decrementar a indentacao */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces faz a indentacao imprimindo espacos */
static void printSpaces(void) {
  int i;
  for (i = 0; i < indentno; i++)
    fprintf(listing, " ");
}

/* A funcao printTree imprime a arvore sintatica para o arquivo
 * listing usando indentacao para indicar as sub-arvores
 */
void printTree( TreeNode * tree ) {
  int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind == StmtK) {
      switch (tree->kind.stmt) {
        case DeclK:
          fprintf(listing, "Declaracao de %s: %s, %s, porta %d\n",
                  nomeCategoria(tree->categoria), tree->attr.name,
                  nomeTipoDisp(tree->tipoDisp), tree->porta);
          break;
        case IfK:
          fprintf(listing, "Comando se\n");
          break;
        case WhileK:
          fprintf(listing, "Comando enquanto\n");
          break;
        case AssignK:
          fprintf(listing, "Atribuicao a: %s\n", tree->attr.name);
          break;
        case ReadK:
          fprintf(listing, "Leitura de: %s\n", tree->attr.name);
          break;
        case WriteK:
          fprintf(listing, "Escrita\n");
          break;
        default:
          fprintf(listing, "Tipo de no de comando desconhecido\n");
          break;
      }
    }
    else if (tree->nodekind == ExpK) {
      switch (tree->kind.exp) {
        case OpK:
          fprintf(listing, "Operador: ");
          printToken(tree->attr.op, "\0");
          break;
        case ConstK:
          fprintf(listing, "Constante: %d\n", tree->attr.val);
          break;
        case IdK:
          fprintf(listing, "Identificador: %s\n", tree->attr.name);
          break;
        default:
          fprintf(listing, "Tipo de no de expressao desconhecido\n");
          break;
      }
    }
    else fprintf(listing, "Tipo de no desconhecido\n");
    for (i = 0; i < MAXCHILDREN; i++)
      printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}
