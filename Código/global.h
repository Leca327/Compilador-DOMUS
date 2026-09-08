#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = quantidade maxima de palavras reservadas */
#define MAXRESERVED 13

typedef enum {
    /* tokens de controle */
    FIMARQ, ERROR,
    /* palavras reservadas */
    SENSOR, ATUADOR, PORTA, ANALOGICO, DIGITAL,
    SE, ENTAO, SENAO, FIM_SE,
    ENQUANTO, FIM_ENQUANTO,
    LER, ESCREVER,
    /* tokens multicaractere */
    ID, NUM,
    /* simbolos especiais */
    ATRIB,               /* <-  */
    IG, DIF,               /* == != */
    ME, MEQ, MA, MAQ,       /* <  <= >  >= */
    MAIS, MENOS, VEZES, DIV,
    EPAREN, DPAREN,
    PONT, DPONT, VIRG     /* . : , */
} TokenType;

extern FILE *source;  /* arquivo de codigo fonte */
extern FILE *listing; /* arquivo com texto de saida */

extern int lineno;    /* numero da linha do codigo fonte */

/*****************************************************/
/***********   Flags para rastreamento    ************/
/*****************************************************/

/* EchoSource = TRUE faz o programa fonte ser impresso no
   arquivo listing com o numero de linha durante a analise */
extern int EchoSource;

/* TraceScan = TRUE faz a informacao do token ser impressa
   quando cada token eh reconhecido pela varredura */
extern int TraceScan;

/* Error = TRUE previne passadas futuras se ocorrer um erro */
extern int Error;

#endif
