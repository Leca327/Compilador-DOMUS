# Transpilador da Linguagem Domus para a Linguagem C

Transpilador completo da linguagem **Domus**, uma linguagem didática de automação
residencial, para a linguagem **C**. Desenvolvido em C com os geradores automáticos
**Flex** e **Bison**, como Trabalho 2 da disciplina de Compiladores do curso de
Engenharia da Computação da Universidade Veiga de Almeida.

O programa lê um arquivo fonte em Domus e executa as quatro fases de um compilador:

| Fase | Responsável | Produz |
|---|---|---|
| Análise léxica | `domus.l` (Flex) | tokens |
| Análise sintática | `domus.y` (Bison) | árvore sintática |
| Análise semântica | `analyze.c` | tabela de símbolos |
| Geração de código | `cgen.c` / `code.c` | programa em C padrão |

Ao final são gravados o `listing.txt`, com os tokens reconhecidos, a árvore sintática
e a tabela de símbolos, e um arquivo `.c` com o programa traduzido. Erros léxicos,
sintáticos e semânticos são informados com o tipo e a linha em que ocorreram.

Este trabalho dá continuidade ao analisador léxico do Trabalho 1.

## A linguagem Domus

Em Domus o programador declara sensores e atuadores ligados a portas e controla seu
comportamento com estruturas de decisão e repetição. Todas as variáveis são inteiras
e são criadas na primeira atribuição, sem seção de declaração.

```domus
/* Programa de exemplo */
sensor temperatura: analogico(porta 1).
atuador cortina: digital(porta 3).

ler(limite).
se temperatura < limite entao
    cortina <- 1.
senao
    cortina <- 0.
fim_se .
escrever(limite).
```

Sensores são de somente leitura e atuadores de somente escrita; cada dispositivo
ocupa uma porta exclusiva. Essas regras são verificadas pela análise semântica.

## Estrutura do projeto

| Arquivo | Descrição |
|---|---|
| `domus.l` | Especificação léxica de entrada do Flex: expressões regulares, ações e a função `getToken()` |
| `domus.y` | Especificação sintática de entrada do Bison: produções, ações que montam a árvore e `yyerror()` |
| `global.h` | Tipos da árvore sintática (`TreeNode`), variáveis globais e flags de rastreamento |
| `scan.h` | Interface do analisador léxico: `MAXTOKENLEN`, `tokenString` e protótipo de `getToken()` |
| `parse.h` | Interface do analisador sintático: protótipo de `parse()` |
| `util.h` / `util.c` | `printToken()`, construção dos nós da árvore e `printTree()` |
| `symtab.h` / `symtab.c` | Tabela de símbolos: tabela de dispersão com `st_insert()`, `st_bucket()` e `printSymTab()` |
| `analyze.h` / `analyze.c` | Análise semântica: `buildSymtab()` e as regras da linguagem |
| `code.h` / `code.c` | Utilitários de emissão de código e controle de indentação |
| `cgen.h` / `cgen.c` | Gerador de código C: `genStmt()`, `genExp()`, `cGen()` e `codeGen()` |
| `main.c` | Programa principal: coordena as quatro fases |
| `prog_teste2.dms` | Programa de teste em Domus |

A organização dos módulos segue a do compilador TINY descrito por Louden (2004).

Arquivos gerados durante o build (`lex.yy.c`, `domus.tab.c`, `domus.tab.h`, `*.o`,
`domusc`, `listing.txt` e o `.c` produzido pelo transpilador) não são versionados.

## Requisitos

- Um compilador C (`gcc`, MinGW, clang)
- O gerador léxico `flex`
- O gerador sintático `bison`

**Linux (Debian/Ubuntu):**

```bash
sudo apt install build-essential flex bison
```

**Windows (MSYS2):** instale o [MSYS2](https://www.msys2.org), execute no terminal
UCRT64 `pacman -S mingw-w64-ucrt-x86_64-gcc flex bison` e acrescente
`C:\msys64\ucrt64\bin` e `C:\msys64\usr\bin` ao PATH.

## Como compilar

```bash
bison -d domus.y
flex domus.l
gcc -o domusc main.c util.c symtab.c analyze.c code.c cgen.c lex.yy.c domus.tab.c
```

O `bison` é executado primeiro porque a opção `-d` gera o arquivo `domus.tab.h`, que
contém os códigos dos tokens e é incluído pelo `global.h`. Sem ele, o analisador
léxico não compila.

## Como executar

```bash
./domusc prog_teste2.dms
```

No Windows (PowerShell), use `.\domusc prog_teste2.dms`. Se nenhum argumento for
informado, o programa abre `prog_teste2.dms` por padrão.

A execução produz o `listing.txt`, com os tokens, a árvore sintática e a tabela de
símbolos, e o `prog_teste2.c`, com o código gerado. Para compilar e executar o
programa traduzido:

```bash
gcc -o prog_teste2 prog_teste2.c
./prog_teste2
```

> O arquivo `.c` gerado é gravado ao lado dos fontes e possui a sua própria função
> `main`. Compile sempre listando os arquivos explicitamente, como acima; um padrão
> como `gcc *.c` incluiria o programa gerado e duplicaria a `main`.

### Exemplo de saída

Tabela de símbolos exibida na tela e gravada no `listing.txt`:

```
Nome             Categoria  Tipo       Porta  Local  Decl.  Linhas de uso
---------------- ---------- ---------- -----  -----  -----  -------------
temperatura      sensor     analogico  1      0      2      13
claridade        sensor     digital    2      1      3      20
cortina          atuador    digital    3      2      8      14 17 20
ventilador       atuador    analogico  4      3      9      15 18
x                variavel   inteiro    -      4      11     21
w                variavel   inteiro    -      5      12
asd              variavel   inteiro    -      6      21     22 23
```

Trecho do código em C gerado:

```c
#include <stdio.h>

/* Sensores, atuadores e variaveis do programa */
int temperatura;      /* sensor analogico - porta 1 */
int cortina;          /* atuador digital - porta 3 */
int x;                /* variavel inteira */

int main(void)
{
    scanf("%d", &x);
    if (temperatura < 30) {
        cortina = 1;
    } else {
        cortina = 0;
    }
    printf("%d\n", (x * 2));

    return 0;
}
```

## Detalhes de implementação

### Análise sintática

- **Gramática LALR(1) sem conflitos.** As produções reproduzem a gramática do
  documento de regras da linguagem. A recursão à esquerda foi mantida, por ser a
  forma natural para um analisador ascendente. A verificação com `bison -Wall` não
  acusa nenhum conflito.
- **Ligação com o analisador léxico.** O `getToken()` do Trabalho 1 permaneceu
  idêntico. O Bison chama uma função `yylex()` declarada como estática no `domus.y`,
  que apenas devolve `getToken()` — a mesma técnica usada no `tiny.y` de Louden.
- **Ações no meio da regra.** O `YYSTYPE` é um `TreeNode *` e não comporta o lexema
  de um identificador nem o valor de um número. Esses valores são gravados em
  `savedName`, `savedPorta` e `savedTipo` por ações colocadas no meio da regra, que
  o Bison executa antes de ler o token seguinte.

### Análise semântica

As seis regras do documento da linguagem são verificadas:

1. um identificador não pode ser declarado duas vezes;
2. cada sensor ou atuador ocupa uma porta exclusiva;
3. a variável é criada na primeira atribuição ou no comando `ler()`;
4. usar uma variável ainda não definida é erro;
5. sensor é de somente leitura — gravar nele é erro;
6. atuador é de somente escrita — ler dele é erro.

A varredura é feita em pré-ordem, com uma exceção: na atribuição, a expressão do
lado direito é analisada antes do identificador do lado esquerdo, para que
`x <- x + 1` com `x` indefinido seja acusado.

### Geração de código

- **Estrutura do programa gerado.** As inclusões, em seguida os sensores, atuadores
  e variáveis como inteiros globais e, por último, a `main()`. As declarações saem
  da tabela de símbolos, na ordem de inserção, porque as variáveis da linguagem
  Domus não possuem declaração no programa fonte.
- **Parênteses nas subexpressões.** A árvore não guarda os parênteses do programa
  fonte, pois a precedência já está na sua forma. Para preservar a ordem de
  avaliação, toda subexpressão é emitida entre parênteses: `w <- (6+8)*(5-7)`
  resulta em `w = ((6 + 8) * (5 - 7));`.
- **Sem `backpatching`.** No TINY o alvo é o código de máquina do simulador TM, e os
  endereços de desvio precisam ser preenchidos retroativamente. Como o alvo aqui é o
  C, que já possui `if` e `while`, esse mecanismo foi dispensado; o `code.c` apenas
  emite texto e controla a indentação.
- O código gerado compila com `gcc -Wall -Wextra -pedantic` sem nenhum aviso.

### Tratamento de erros

Cada erro é informado com o tipo e a linha:

```
Erro lexico (linha 4): caractere invalido '@'
Erro sintatico (linha 6): nao era esperado ESCREVER; era esperado ENTAO
Erro semantico (linha 6): tentativa de gravacao no sensor "temperatura"; sensores sao de somente leitura
```

Erros léxicos e sintáticos são detectados durante a análise sintática e podem ser
reportados juntos, graças às produções `error` do Bison. Eles interrompem a
compilação antes da análise semântica, porque a árvore não pode ser considerada
confiável. Os erros semânticos não interrompem a varredura: todos são reportados e
a tabela de símbolos é exibida, mas nenhum código é gerado.

## Referências

- LOUDEN, K. C. *Compiladores: princípios e práticas*. São Paulo: Cengage Learning, 2004.
- AHO, A. V. et al. *Compiladores: princípios, técnicas e ferramentas*. 2. ed.
  São Paulo: Pearson Addison-Wesley, 2008.
- [Manual do Flex](https://westes.github.io/flex/manual/)
- [Manual do Bison](https://www.gnu.org/software/bison/manual/)
