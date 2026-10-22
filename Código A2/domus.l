/****************************************************/
/* Arquivo: domus.l                                 */
/* Especificacao Lexica para DOMUS (entrada do FLEX)*/
/****************************************************/

/* A funcao yywrap() eh chamada automaticamente no final do arquivo. */
/* A opcao abaixo evita este comportamento. */
%option noyywrap

%{
#include "global.h"
#include "util.h"
#include "scan.h"
/* lexema do identificador ou palavra reservada */
char tokenString[MAXTOKENLEN+1];

/* sinaliza que o arquivo terminou com um comentario em aberto */
static int comentarioAberto = FALSE;
%}

digito       [0-9]
numero       {digito}+
letra        [a-zA-Z_]
identificador {letra}({letra}|{digito})*
newline      \n
espaco       [ \t\r]+

%%

"sensor"          {return SENSOR;}
"atuador"         {return ATUADOR;}
"porta"           {return PORTA;}
"analogico"       {return ANALOGICO;}
"digital"         {return DIGITAL;}
"se"              {return SE;}
"entao"           {return ENTAO;}
"senao"           {return SENAO;}
"fim_se"          {return FIM_SE;}
"enquanto"        {return ENQUANTO;}
"fim_enquanto"    {return FIM_ENQUANTO;}
"ler"             {return LER;}
"escrever"        {return ESCREVER;}
"<-"              {return ATRIB;}
"=="              {return IG;}
"!="              {return DIF;}
"<="              {return MEQ;}
">="              {return MAQ;}
"<"               {return ME;}
">"               {return MA;}
"+"               {return MAIS;}
"-"               {return MENOS;}
"*"               {return VEZES;}
"/"               {return DIV;}
"("               {return EPAREN;}
")"               {return DPAREN;}
"."               {return PONT;}
":"               {return DPONT;}
","               {return VIRG;}
{numero}          {return NUM;}
{identificador}   {return ID;}
{newline}         {lineno++;}
{espaco}          {/* ignora espaco em branco */}
"/*"              {
                    int c, prev = 0;
                    for (;;) {
                      c = input();
                      if (c == EOF || c == 0) {
                        comentarioAberto = TRUE;
                        return ERROR;
                      }
                      if (c == '\n') lineno++;
                      if (prev == '*' && c == '/') break;
                      prev = c;
                    }
                  }
.                 {return ERROR;}

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  /* a mensagem nao pode ser escrita em yytext, que pertence ao flex */
  if (currentToken == ERROR)
  { if (comentarioAberto)
    { strcpy(tokenString, "comentario aberto nao foi fechado");
      comentarioAberto = FALSE;
    }
    else
      sprintf(tokenString, "caractere invalido '%c'", yytext[0]);
  }
  else
  { strncpy(tokenString, yytext, MAXTOKENLEN);
    tokenString[MAXTOKENLEN] = '\0';
  }
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}
