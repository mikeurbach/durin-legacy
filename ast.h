#ifndef __AST_H_
#define __AST_H_ 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define true 1
#define false 0
typedef int bool;

/* 
   these are the types of nodes in the AST, terminals in the abstract syntax
 */
typedef enum {
  IDENTIFIER,
  INTEGER,
  FLOAT,
  STRING,
  BOOL,
  BINDVAR,
  BINDFUN,
  OROP,
  ANDOP,
  EQOP, 
  INEQOP,
  LTOP, 
  GTOP, 
  LTEOP, 
  GTEOP,
  ADDOP, 
  EADDOP, 
  SUBOP, 
  ESUBOP,
  NEGOP,
  ENEGOP,
  MULOP, 
  EMULOP, 
  DIVOP, 
  EDIVOP, 
  MODOP, 
  EMODOP,
  EXPOP, 
  EEXPOP,
  NOTOP
} astnode_type;


/* 
   for printing ASTs
 */
struct token_lookup {
  char *token;
  astnode_type node_type;
};

static struct token_lookup token_table[] = {
  {"IDENTIFIER",IDENTIFIER},
  {"INTEGER",INTEGER},
  {"FLOAT",FLOAT},
  {"STRING",STRING},
  {"BOOL",BOOL},
  {"BIND VAR",BINDVAR},
  {"BIND FUN",BINDFUN},
  {"||",OROP},
  {"&&",ANDOP},
  {"==",EQOP}, 
  {"!=",INEQOP},
  {"<",LTOP}, 
  {">",GTOP}, 
  {"<=",LTEOP}, 
  {">=",GTEOP},
  {"+",ADDOP}, 
  {".+",EADDOP}, 
  {"-",SUBOP}, 
  {".-",ESUBOP},
  {"-",NEGOP}, 
  {".-",ENEGOP},
  {"*",MULOP}, 
  {".*",EMULOP}, 
  {"/",DIVOP}, 
  {"./",EDIVOP}, 
  {"%",MODOP}, 
  {".%",EMODOP},
  {"^",EXPOP}, 
  {".^",EEXPOP},
  {"!",NOTOP},
  {NULL, 0}
};

/* 
   core AST node structure
 */
typedef struct astnode * astnode;
struct astnode {
  astnode_type type;
  union {
    int integer_val;
    float float_val;
    char *string_val;
    bool bool_val;
  } value;
  astnode lchild;
  astnode rsibling;
};

#define YYSTYPE astnode // for lexer and parser

astnode create_astnode(int);
void print_ast(astnode,int);

#endif
