#include "codegen.h"

symboltable symtab;

static int program(astnode node, void *data){
  return 0;
}

/* handles each node of the abstract sytax tree */
static int codegenerator(astnode node, void *data){
  int errors = 0;

  /* big switch to delegate to helpers */
  switch(node->type){
  case PROGRAM:
    errors += program(node, data);
    break;
  case STATEMENT:
    break;
  case IDENTIFIER:
    break;
  case INTEGER:
    break;
  case FLOAT:
    break;
  case STRING:
    break;
  case BOOL:
    break;
  case RANGE:
    break;
  case INDEX:
    break;
  case SUBSCRIPT:
    break;
  case BINDVAR:
    break;
  case BINDFUN:
    break;
  case PARAMS:
    break;
  case RETURN:
    break;
  case FUNCALL:
    break;
  case INDEXEDBLOCK:
    break;
  case TERNARYOP:
    break;
  case OROP:
    break;
  case ANDOP:
    break;
  case EQOP:
    break;
  case INEQOP:
    break;
  case LTOP:
    break;
  case GTOP:
    break;
  case LTEOP:
    break;
  case GTEOP:
    break;
  case ADDOP:
    break;
  case EADDOP:
    break;
  case SUBOP:
    break;
  case ESUBOP:
    break;
  case NEGOP:
    break;
  case ENEGOP:
    break;
  case MULOP:
    break;
  case EMULOP:
    break;
  case DIVOP:
    break;
  case EDIVOP:
    break;
  case MODOP:
    break;
  case EMODOP:
    break;
  case EXPOP:
    break;
  case EEXPOP:
    break;
  case NOTOP:
    break;
  default:
    break;
  }

  return errors;
}

/* main entry point for code generation */
void codegen(astnode root){
  int count = traverse_ast(root, codegenerator, NULL);
}
