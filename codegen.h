#ifndef __CODEGEN_H_
#define __CODEGEN_H_ 1

#include <string.h>
#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>
#include "durin.h"
#include "ast.h"

#define MAXLINE 1024
#define itod64(i) (uint64_t) (i | 0x0000000000000000) << 8;
#define d64toi(d) (int) (d >> 8) | 0x00000000;

/* 
   structs for creating datapaths
 */
typedef struct datapath_node * datapath_node;
struct datapath_node {
  Agrec_t hdr;
  uint64_t value;
  astnode_type operator;
};

#include "symtab.h" // has to be after datapath_node declared

void codegen(astnode);

#endif
