# Analisador Léxico da Linguagem Domus

Analisador léxico (scanner) para a linguagem **Domus**, uma linguagem didática de
automação residencial. Desenvolvido em C com o gerador automático **Flex** como
Trabalho 1 da disciplina de Compiladores do curso de Engenharia da Computação da
Universidade Veiga de Almeida.

O programa lê um arquivo fonte em Domus, reconhece os tokens da linguagem e grava
em `listing.txt` cada token encontrado, seu atributo (quando houver) e a linha do
programa fonte onde ele aparece.

## A linguagem Domus

Em Domus o programador declara sensores e atuadores ligados a portas e controla
seu comportamento com estruturas de decisão e repetição:

```domus
/* Programa de exemplo */
sensor temperatura: analogico(porta 1).
atuador cortina: digital(porta 3).

se (temperatura < 30) entao
    cortina <- 1.
senao
    cortina <- 0.
fim_se .
```

## Estrutura do projeto

| Arquivo | Descrição |
|---|---|
| `domus.l` | Especificação léxica de entrada do Flex: expressões regulares, ações e a função `getToken()` |
| `global.h` | Enumeração `TokenType`, variáveis globais e flags de rastreamento |
| `scan.h` | Interface do analisador: `MAXTOKENLEN`, `tokenString` e protótipo de `getToken()` |
| `util.h` / `util.c` | Função `printToken()`, que exibe textualmente cada token e seu atributo |
| `main.c` | Programa principal: abre o fonte e chama `getToken()` até o fim do arquivo |
| `prog_teste.dms` | Programa de teste em Domus |

Arquivos gerados durante o build (`lex.yy.c`, `*.o`, `domus_lex`, `listing.txt`)
não são versionados.

## Requisitos

- Um compilador C (`gcc`, MinGW, clang)
- O gerador `flex`

**Linux (Debian/Ubuntu):**

```bash
sudo apt install build-essential flex
```

**Windows (MSYS2):** instale o [MSYS2](https://www.msys2.org), execute no terminal
UCRT64 `pacman -S mingw-w64-ucrt-x86_64-gcc flex make` e acrescente
`C:\msys64\ucrt64\bin` e `C:\msys64\usr\bin` ao PATH.

## Como compilar

```manualmente:

```bash
flex domus.l
gcc -o domus_lex main.c util.c lex.yy.c
```

## Como executar

```bash
./domus_lex prog_teste.dms
```

No Windows (PowerShell), use `.\domus_lex prog_teste.dms`. Se nenhum argumento for
informado, o programa abre `prog_teste.dms` por padrão. O resultado da análise é
sempre gravado em `listing.txt`.

### Exemplo de saída

```
    2: palavra reservada: sensor
    2: ID, nome= temperatura
    2: SEPARADOR_TIPO: :
    2: palavra reservada: analogico
    2: ABRE_PARENTESES: (
    2: palavra reservada: porta
    2: NUM, val= 1
    2: FECHA_PARENTESES: )
    2: SEPARADOR_COMANDO: .
   ...
   24: EOF (fim de arquivo)
```

## Detalhes de implementação

- **Palavras reservadas antes de identificadores.** Como `_` faz parte da classe
  `letra`, lexemas como `fim_se` também casam com o padrão de identificador. A
  ordem das regras no `domus.l` garante que sejam reconhecidos como palavra
  reservada.
- **Maior casamento (longest match).** Símbolos que são prefixo de outros
  (`<` em `<-` e `<=`) são resolvidos pela regra do lexema mais longo do Flex.
- **Comentários.** A ação da regra `"/*"` consome caracteres com `input()` até
  encontrar `*/`, contando quebras de linha. Comentários podem ocupar várias
  linhas, mas não podem ser aninhados; se o arquivo terminar antes do fechamento,
  é emitido um erro léxico.
- **Erros não interrompem a varredura.** Qualquer caractere não reconhecido gera
  o token `ERROR` com o lexema como atributo, e a análise segue até o fim do
  arquivo, reportando todos os erros de uma vez.

## Referências

- LOUDEN, K. C. *Compiladores: princípios e práticas*. São Paulo: Pioneira
  Thomson Learning, 2004.
- AHO, A. V. et al. *Compiladores: princípios, técnicas e ferramentas*. 2. ed.
  São Paulo: Pearson Addison-Wesley, 2008.
- [Manual do Flex](https://westes.github.io/flex/manual/)
