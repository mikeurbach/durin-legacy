#ifndef __AST_H_
#define __AST_H_ 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define true 1
#define false 0
typedef int bool;

/* 
   these are the types of nodes in the AST, terminals in the abstract syntax
 */
typedef enum {
  PROGRAM,
  STATEMENT,
  IDENTIFIER,
  INTEGER,
  FLOAT,
  STRING,
  BOOL,
  RANGE,
  INDEX,
  SUBSCRIPT,
  BINDVAR,
  BINDFUN,
  PARAMS,
  RETURN,
  FUNCALL,
  INDEXEDBLOCK,
  TERNARYOP,
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
  {"PROGRAM",PROGRAM},
  {"STATEMENT",STATEMENT},
  {"IDENTIFIER",IDENTIFIER},
  {"INTEGER",INTEGER},
  {"FLOAT",FLOAT},
  {"STRING",STRING},
  {"BOOL",BOOL},
  {"RANGE",RANGE},
  {"INDEX",INDEX},
  {"SUBSCRIPT",SUBSCRIPT},
  {"BINDVAR",BINDVAR},
  {"BINDFUN",BINDFUN},
  {"PARAMS",PARAMS},
  {"RETURN",RETURN},
  {"FUNCALL",FUNCALL},
  {"INDEXEDBLOCK",INDEXEDBLOCK},
  {"TERNARYOP",TERNARYOP},
  {"OR",OROP},
  {"AND",ANDOP},
  {"EQ",EQOP},
  {"INEQ",INEQOP},
  {"LT",LTOP},
  {"GT",GTOP},
  {"LTE",LTEOP},
  {"GTE",GTEOP},
  {"ADD",ADDOP},
  {"EADD",EADDOP},
  {"SUB",SUBOP},
  {"ESUB",ESUBOP},
  {"NEG",NEGOP},
  {"ENEG",ENEGOP},
  {"MUL",MULOP},
  {"EMUL",EMULOP},
  {"DIV",DIVOP},
  {"EDIV",EDIVOP},
  {"MOD",MODOP},
  {"EMOD",EMODOP},
  {"EXP",EXPOP},
  {"EEXP",EEXPOP},
  {"NOT",NOTOP},
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
    char *bool_val;
  } value;
  int lower_bound;
  int upper_bound;
  astnode lchild;
  astnode rsibling;
};

#define YYSTYPE astnode // for lexer and parser

astnode create_astnode(int);
int print_ast(astnode);
int destroy_ast(astnode);
int traverse_ast_up(astnode, int (*)(astnode, void*), void *);
int traverse_ast_down(astnode, int (*)(astnode, void*), void *);

#endif
