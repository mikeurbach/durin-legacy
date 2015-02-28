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
  DECIMAL,
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

const static struct token_lookup token_table[] = {
  {"program",PROGRAM},
  {"statement",STATEMENT},
  {"identifier",IDENTIFIER},
  {"integer",INTEGER},
  {"float",FLOAT},
  {"string",STRING},
  {"bool",BOOL},
  {"range",RANGE},
  {"index",INDEX},
  {"subscript",SUBSCRIPT},
  {"bindvar",BINDVAR},
  {"bindfun",BINDFUN},
  {"params",PARAMS},
  {"return",RETURN},
  {"funcall",FUNCALL},
  {"indexedblock",INDEXEDBLOCK},
  {"ternary",TERNARYOP},
  {"or",OROP},
  {"and",ANDOP},
  {"eq",EQOP},
  {"ineq",INEQOP},
  {"lt",LTOP},
  {"gt",GTOP},
  {"lte",LTEOP},
  {"gte",GTEOP},
  {"add",ADDOP},
  {"eadd",EADDOP},
  {"sub",SUBOP},
  {"esub",ESUBOP},
  {"neg",NEGOP},
  {"eneg",ENEGOP},
  {"mul",MULOP},
  {"emul",EMULOP},
  {"div",DIVOP},
  {"ediv",EDIVOP},
  {"mod",MODOP},
  {"emod",EMODOP},
  {"exp",EXPOP},
  {"eexp",EEXPOP},
  {"not",NOTOP},
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
