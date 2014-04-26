#ifndef __SYMTAB_H_
#define __SYMTAB_H_ 1

#include "ast.h"

#define SYMTAB_SIZE 1024

/* struct for the node in the symbol table */
typedef struct symnode * symnode;
struct symnode {
  char *identifier;
  astnode_type type;
  int dim_count;
  int param_count;
  int return_count;
  int *dims;
  symnode *params;
  symnode *returns;
  symnode next;
};

typedef struct symhashtable *symhashtable;
struct symhashtable {
  int level;
  symnode *table;
  symhashtable outer_scope;
};

typedef struct symboltable *symboltable;
struct symboltable {
  symhashtable inner_scope;
};

symnode create_symnode(char *, int);
symboltable create_symboltable();
void destroy_symboltable(symboltable);
symnode lookup_symnode(symboltable, char *);
symnode insert_symnode(symboltable, symnode);
void enter_scope(symboltable);
void leave_scope(symboltable);

#endif
