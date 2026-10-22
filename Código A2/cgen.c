/*************************************************************/
/* Arquivo: cgen.c                                           */
/* Implementacao do gerador de codigo para o compilador DOMUS*/
/* Baseado em: Compiler Construction                         */
/* Kenneth C. Louden                                         */
/*                                                           */
/* O alvo da traducao eh a linguagem C padrao. O codigo       */
/* gerado tem a estrutura exigida pelo documento de regras   */
/* da linguagem: as inclusoes, em seguida os sensores, os    */
/* atuadores e as variaveis como inteiros globais e, por     */
/* ultimo, a funcao main() com os comandos traduzidos.       */
/*************************************************************/

#include "global.h"
#include "util.h"
#include "symtab.h"
#include "code.h"
#include "cgen.h"

/* Prototipo para a funcao recursiva de geracao de codigo */
static void cGen(TreeNode *tree);

/* Devolve o operador da linguagem C correspondente ao token */
static char *operadorC(TokenType op) {
  switch (op) {
    case MAIS:  return "+";
    case MENOS: return "-";
    case VEZES: return "*";
    case DIV:   return "/";
    case ME:    return "<";
    case MEQ:   return "<=";
    case MA:    return ">";
    case MAQ:   return ">=";
    case IG:    return "==";
    case DIF:   return "!=";
    default:    return "?";
  }
}

/* Gera codigo para um no de comando */
static void genStmt(TreeNode *tree) {
  TreeNode *p1, *p2, *p3;
  switch (tree->kind.stmt) {

      case DeclK: /* DECLARACAO DE SENSOR OU ATUADOR */
         /* nada a gerar aqui: os dispositivos ja foram declarados
            como variaveis globais no inicio do programa */
         break;

      case IfK: /* CODIGO PARA O COMANDO SE */
         if (TraceCode)
           emitComment("-> SE");
         p1 = tree->child[0]; /* expressao de teste */
         p2 = tree->child[1]; /* bloco da parte entao */
         p3 = tree->child[2]; /* bloco da parte senao  */
         emitIndent();
         emitString("if ");
         cGen(p1); /* Gera codigo da expressao de teste */
         emitString(" {");
         emitNewLine();
         emitIncIndent();
         cGen(p2); /* Gera codigo da parte entao */
         emitDecIndent();
         if (p3 != NULL) { /* Se existir a parte senao */
           emitLine("} else {");
           emitIncIndent();
           cGen(p3); /* Gera codigo da parte senao */
           emitDecIndent();
         }
         emitLine("}");
         if (TraceCode)
           emitComment("<- SE");
         break; /* IfK */

      case WhileK: /* CODIGO PARA O COMANDO ENQUANTO */
         if (TraceCode)
           emitComment("-> ENQUANTO");
         p1 = tree->child[0]; /* expressao de teste */
         p2 = tree->child[1]; /* corpo da repeticao  */
         emitIndent();
         emitString("while ");
         cGen(p1); /* Gera codigo da expressao de teste */
         emitString(" {");
         emitNewLine();
         emitIncIndent();
         cGen(p2); /* Gera codigo do corpo da repeticao */
         emitDecIndent();
         emitLine("}");
         if (TraceCode)
           emitComment("<- ENQUANTO");
         break; /* WhileK */

      case AssignK: /* CODIGO PARA O COMANDO DE ATRIBUICAO */
         if (TraceCode)
           emitComment("-> ATRIBUICAO");
         emitIndent();
         emitString(tree->attr.name);
         emitString(" = ");
         cGen(tree->child[0]); /* Gera codigo do lado direito */
         emitString(";");
         emitNewLine();
         if (TraceCode)
           emitComment("<- ATRIBUICAO");
         break; /* AssignK */

      case ReadK: /* CODIGO PARA O COMANDO LER */
         emitIndent();
         emitString("scanf(\"%d\", &");
         emitString(tree->attr.name);
         emitString(");");
         emitNewLine();
         break; /* ReadK */

      case WriteK: /* CODIGO PARA O COMANDO ESCREVER */
         emitIndent();
         emitString("printf(\"%d\\n\", ");
         cGen(tree->child[0]); /* Gera codigo da expressao */
         emitString(");");
         emitNewLine();
         break; /* WriteK */

      default:
         break;
    }
} /* genStmt */

/* Gera codigo para um no de expressao */
static void genExp(TreeNode *tree) {
  switch (tree->kind.exp) {

    case ConstK: /* CONSTANTE */
      emitNum(tree->attr.val);
      break; /* ConstK */

    case IdK: /* IDENTIFICADOR */
      emitString(tree->attr.name);
      break; /* IdK */

    case OpK: /* OPERADOR */
      /* A arvore sintatica nao guarda os parenteses do programa
         fonte, pois a precedencia ja esta representada pela sua
         forma. Para que o codigo C avalie a expressao na mesma
         ordem, toda subexpressao eh emitida entre parenteses. */
      emitString("(");
      cGen(tree->child[0]); /* Gera codigo do operando esquerdo */
      emitString(" ");
      emitString(operadorC(tree->attr.op));
      emitString(" ");
      cGen(tree->child[1]); /* Gera codigo do operando direito */
      emitString(")");
      break; /* OpK */

    default:
      break;
  }
} /* genExp */

/* Gera o codigo pela travessia recursiva da arvore sintatica */
static void cGen(TreeNode *tree) {
  if (tree != NULL) {
    switch (tree->nodekind) { /* Verifica o no da arvore */
      case StmtK: /* No de comando */
        genStmt(tree); /* Gera codigo para o comando */
        break;
      case ExpK: /* No de expressao */
        genExp(tree); /* Gera codigo para a expressao */
        break;
      default:
        break;
    }
    cGen(tree->sibling); /* Gera codigo para o comando na sequencia */
  }
}

/* Gera as declaracoes globais a partir da tabela de simbolos */
static void genDeclaracoes(void) {
  BucketList l;
  char decl[64]; /* nome do identificador seguido de ponto e virgula */

  emitLine("/* Sensores, atuadores e variaveis do programa */");
  for (l = st_first(); l != NULL; l = l->nextOrder) {
    sprintf(decl, "%s;", l->name);
    emitIndent();
    fprintf(code, "int %-18s", decl);
    if (l->porta > 0)
      fprintf(code, "/* %s %s - porta %d */\n",
              nomeCategoria(l->categoria), nomeTipoDisp(l->tipo), l->porta);
    else
      fprintf(code, "/* variavel inteira */\n");
  }
}

/***************************************\
* FUNCAO PRINCIPAL DO GERADOR DE CODIGO *
\***************************************/

/* Gera o codigo em linguagem C pela travessia da arvore sintatica */
void codeGen(TreeNode *syntaxTree, char *codefile) {
  /* Comentario inicial contendo o nome do arquivo */
  emitLine("/*****************************************************************/");
  emitLine("/* Codigo em linguagem C gerado pelo compilador DOMUS            */");
  emitIndent();
  fprintf(code, "/* Arquivo: %-53s*/\n", codefile);
  emitLine("/*****************************************************************/");
  emitNewLine();

  /* Inclusoes necessarias a compilacao do programa */
  emitLine("#include <stdio.h>");
  emitNewLine();

  /* Sensores, atuadores e variaveis como inteiros globais */
  genDeclaracoes();
  emitNewLine();

  /* Funcao main() com o codigo Domus traduzido */
  emitLine("int main(void)");
  emitLine("{");
  emitIncIndent();
  cGen(syntaxTree);
  emitNewLine();
  emitLine("return 0;");
  emitDecIndent();
  emitLine("}");
}
