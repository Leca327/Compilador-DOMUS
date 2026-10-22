/**************************************************/
/* Arquivo: domus.y                               */
/* Arquivo de especificacao DOMUS para Yacc/Bison */
/**************************************************/
%{
#define YYPARSER /* Distingue a saida do Yacc de outros arquivos de codigo */

#include "global.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static char * savedName;          /* Para usar nas atribuicoes e declaracoes  */
static int savedLineNo;           /* Temporario para salvar o numero da linha */
static int savedPorta;            /* Temporario para a porta do dispositivo   */
static TipoDispositivo savedTipo; /* Temporario para o tipo do dispositivo    */
static TreeNode * savedTree;      /* Armazena a arvore sintatica para retornar
                                     posteriormente */

static int yylex(void);    /* Remover aviso de declaracao implicita da funcao */
int yyerror(const char *); /* Remover aviso de declaracao implicita da funcao */

/* Anexa um no ao final da lista de irmaos. Usada pelas producoes
   que formam listas: declaracoes, sensores, atuadores e comandos. */
static TreeNode * anexa(TreeNode * lista, TreeNode * novo)
{ TreeNode * t;
  if (lista == NULL) return novo;
  t = lista;
  while (t->sibling != NULL) t = t->sibling;
  t->sibling = novo;
  return lista;
}
%}

%token SENSOR ATUADOR PORTA ANALOGICO DIGITAL
%token SE ENTAO SENAO FIM_SE
%token ENQUANTO FIM_ENQUANTO
%token LER ESCREVER
%token ID NUM
%token ATRIB IG DIF ME MEQ MA MAQ
%token MAIS MENOS VEZES DIV
%token EPAREN DPAREN PONT DPONT VIRG
%token ERROR

%define parse.error verbose

%% /* Gramatica da linguagem DOMUS */

/* Um programa possui uma secao opcional de declaracoes de sensores
   e atuadores, seguida do corpo do programa. */
programa    : declaracao_lista bloco
                 { savedTree = anexa($1,$2); $$ = savedTree; }
            | bloco
                 { savedTree = $1; $$ = savedTree; }
            ;

declaracao_lista : declaracao_lista declaracao
                 { $$ = anexa($1,$2); }
            | declaracao
                 { $$ = $1; }
            ;

declaracao  : declaracao_sensor  { $$ = $1; }
            | declaracao_atuador { $$ = $1; }
            ;

/* ----------------------- sensores ----------------------------- */
declaracao_sensor : SENSOR lista_sensores PONT
                 { $$ = $2; }
            ;

lista_sensores : lista_sensores VIRG sensor
                 { $$ = anexa($1,$3); }
            | sensor
                 { $$ = $1; }
            ;

sensor      : ID { savedName = copyString(tokenString);
                   savedLineNo = lineno; }
              DPONT tipo_sensor_atuador EPAREN PORTA
              NUM { savedPorta = atoi(tokenString); }
              DPAREN
                 { $$ = newStmtNode(DeclK);
                   $$->attr.name = savedName;
                   $$->lineno = savedLineNo;
                   $$->categoria = CatSensor;
                   $$->tipoDisp = savedTipo;
                   $$->porta = savedPorta;
                 }
            ;

tipo_sensor_atuador : ANALOGICO { savedTipo = TipoAnalogico; $$ = NULL; }
            | DIGITAL           { savedTipo = TipoDigital;   $$ = NULL; }
            ;

/* ----------------------- atuadores ---------------------------- */
declaracao_atuador : ATUADOR lista_atuadores PONT
                 { $$ = $2; }
            ;

lista_atuadores : lista_atuadores VIRG atuador
                 { $$ = anexa($1,$3); }
            | atuador
                 { $$ = $1; }
            ;

atuador     : ID { savedName = copyString(tokenString);
                   savedLineNo = lineno; }
              DPONT tipo_sensor_atuador EPAREN PORTA
              NUM { savedPorta = atoi(tokenString); }
              DPAREN
                 { $$ = newStmtNode(DeclK);
                   $$->attr.name = savedName;
                   $$->lineno = savedLineNo;
                   $$->categoria = CatAtuador;
                   $$->tipoDisp = savedTipo;
                   $$->porta = savedPorta;
                 }
            ;

/* ------------------- corpo do programa ------------------------ */
/* O ponto separa os comandos e tambem encerra o ultimo deles.    */
bloco       : bloco comando PONT
                 { $$ = anexa($1,$2); }
            | comando PONT
                 { $$ = $1; }
            ;

comando     : comando_se         { $$ = $1; }
            | comando_enquanto   { $$ = $1; }
            | comando_atribuicao { $$ = $1; }
            | comando_leitura    { $$ = $1; }
            | comando_escrita    { $$ = $1; }
            | error              { $$ = NULL; }
            ;

/* A parte senao eh opcional */
comando_se  : SE expressao_logica ENTAO bloco FIM_SE
                 { $$ = newStmtNode(IfK);
                   $$->child[0] = $2;
                   $$->child[1] = $4;
                 }
            | SE expressao_logica ENTAO bloco SENAO bloco FIM_SE
                 { $$ = newStmtNode(IfK);
                   $$->child[0] = $2;
                   $$->child[1] = $4;
                   $$->child[2] = $6;
                 }
            ;

comando_enquanto : ENQUANTO EPAREN expressao_logica DPAREN bloco FIM_ENQUANTO
                 { $$ = newStmtNode(WhileK);
                   $$->child[0] = $3;
                   $$->child[1] = $5;
                 }
            ;

comando_atribuicao : ID { savedName = copyString(tokenString);
                          savedLineNo = lineno; }
              ATRIB expressao
                 { $$ = newStmtNode(AssignK);
                   $$->child[0] = $4;
                   $$->attr.name = savedName;
                   $$->lineno = savedLineNo;
                 }
            ;

comando_leitura : LER EPAREN ID { savedName = copyString(tokenString);
                                  savedLineNo = lineno; }
              DPAREN
                 { $$ = newStmtNode(ReadK);
                   $$->attr.name = savedName;
                   $$->lineno = savedLineNo;
                 }
            ;

comando_escrita : ESCREVER EPAREN expressao DPAREN
                 { $$ = newStmtNode(WriteK);
                   $$->child[0] = $3;
                 }
            ;

/* ------------------------ expressoes -------------------------- */
/* As producoes op_relacional, soma e multi da gramatica da       */
/* linguagem foram expandidas em uma producao por operador, para  */
/* que cada uma tenha a sua propria acao, como no compilador TINY.*/
expressao_logica : EPAREN expressao_logica DPAREN
                 { $$ = $2; }
            | expressao ME expressao
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = ME;
                 }
            | expressao MEQ expressao
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = MEQ;
                 }
            | expressao MA expressao
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = MA;
                 }
            | expressao MAQ expressao
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = MAQ;
                 }
            | expressao IG expressao
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = IG;
                 }
            | expressao DIF expressao
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = DIF;
                 }
            ;

expressao   : expressao MAIS termo
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = MAIS;
                 }
            | expressao MENOS termo
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = MENOS;
                 }
            | termo { $$ = $1; }
            ;

termo       : termo VEZES fator
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = VEZES;
                 }
            | termo DIV fator
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = DIV;
                 }
            | fator { $$ = $1; }
            ;

fator       : EPAREN expressao DPAREN
                 { $$ = $2; }
            | NUM
                 { $$ = newExpNode(ConstK);
                   $$->attr.val = atoi(tokenString);
                 }
            | ID
                 { $$ = newExpNode(IdK);
                   $$->attr.name = copyString(tokenString);
                 }
            | error { $$ = NULL; }
            ;

%%

/* Troca os trechos em ingles gerados pelo Bison pelo texto em
   portugues, para que a mensagem de erro fique clara. */
static void substituir(char * destino, int tam, const char * origem,
                       const char * de, const char * para)
{ int i = 0;
  int tamDe = (int) strlen(de);
  int tamPara = (int) strlen(para);
  const char * p = origem;
  while (*p != '\0' && i + 1 < tam) {
    if (strncmp(p, de, tamDe) == 0) {
      int k;
      for (k = 0; k < tamPara && i + 1 < tam; k++) destino[i++] = para[k];
      p += tamDe;
    } else {
      destino[i++] = *p++;
    }
  }
  destino[i] = '\0';
}

int yyerror(const char * message)
{ char a[512];
  char b[512];

  /* o analisador lexico devolve ERROR quando encontra um lexema
     invalido; nesse caso o erro e lexico, e nao sintatico */
  if (yychar == ERROR) {
    fprintf(listing, ">>> Erro lexico (linha %d): %s\n", lineno, tokenString);
    printf("Erro lexico (linha %d): %s\n", lineno, tokenString);
    Error = TRUE;
    return 0;
  }

  substituir(a, (int) sizeof a, message, "syntax error, unexpected", "nao era esperado");
  substituir(b, (int) sizeof b, a, ", expecting", "; era esperado");
  substituir(a, (int) sizeof a, b, " or ", " ou ");
  substituir(b, (int) sizeof b, a, "end of file", "o fim do arquivo");
  substituir(a, (int) sizeof a, b, "syntax error", "construcao invalida");

  fprintf(listing, ">>> Erro sintatico (linha %d): %s\n", lineno, a);
  fprintf(listing, "    Token atual: ");
  printToken(yychar, tokenString);
  printf("Erro sintatico (linha %d): %s\n", lineno, a);
  Error = TRUE;
  return 0;
}

/* yylex chama getToken() para fazer a saida do Yacc/Bison
 * compativel com versoes anteriores do analisador lexico
 * da linguagem Domus.
 * Precisa ser definida como estatica para ser chamada
 * por yyparse() no lugar da yylex() global criada pelo
 * flex.
 * A funcao getToken() chamara a yylex() do flex.
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}
