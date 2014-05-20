#include "codegen.h"

symboltable symtab;
Agraph_t *datapath, *current_dp;

/* declarations and helpers */
int handlenode(astnode node, void *data);

char *dpname(astnode node){
  char *strval = malloc(64);
  memset(strval, 0, 64);

  switch(node->type){
  case INTEGER:
    snprintf(strval, 64, "%d", node->value.integer_val);
    break;
  case IDENTIFIER:
    snprintf(strval, 64, "%s", node->value.string_val);
    break;
  default:
    snprintf(strval, 64, "%s", token_table[node->type].token);
    break;
  }
  
  return strval;
}

/* 
   the functions at the top of this (before the big handlenode switch)
   are called roughly in order top to bottom and up again to traverse AST
*/

/* 
   instantiates the global symbol table and shared datapath, then
   sets up some graph attributes for outputting the datapath
*/
static int program_down(astnode node, void *data){
    symtab = create_symboltable();
    datapath = current_datapath(symtab);

    agattr(datapath, AGNODE, "shape", "box");
    agattr(datapath, AGRAPH, "rankdir", "LR");

    return (!symtab || !datapath) ? 1 : 0;
}

/* 
   enters a new scope, puts the function name, inputs, 
   and outputs in the symbol table. also creates datapath
   nodes for the inputs and outputs
*/
static int bindfun_down(astnode node, void *data){
  int errors = 0;
  astnode inout, portname;
  symnode snode;
  Agraph_t *dp;
  Agnode_t *dpnode;
  datapath_node dprec;

  /* enter a new scope, which gets its own datapath */
  enter_scope(symtab);

  /* add the module name to the symbol table, if it doesn't exist,
     and then names a subgraph in the current scope's datapath
     else this is a semantic error */
  if(!lookup_symnode(symtab, node->lchild->value.string_val)){
    snode = create_symnode(node->lchild->value.string_val, BINDFUN);
    insert_symnode(symtab, snode);
    dp = agsubg(datapath, snode->identifier, TRUE);
    current_dp = dp;
  } else {
    printf("symbol %s has been used\n", node->lchild->value.string_val);
    errors += 1;
  }

  /* go through the inputs adding them to the symbol table and datapath */
  inout = node->lchild->rsibling;
  portname = inout->lchild;
  if(portname){
    while(portname){
      /* if the input is not in the symbol table already add it, 
	 else this is a semantic error */
      if(!lookup_symnode(symtab, portname->value.string_val)){
	snode = create_symnode(portname->value.string_val, PARAMS);
	insert_symnode(symtab, snode);
	dpnode = agnode(dp, snode->identifier, TRUE);
	dprec = (datapath_node) agbindrec(dpnode, "datapath_node",
					  sizeof(struct datapath_node),
					  FALSE);
	dprec->operator = PARAMS;
      } else {
	printf("symbol %s has been used\n", portname->value.string_val);
	errors += 1;
      }
      portname = portname->rsibling;
    }
  }
    
  /* go through the outputs, adding them to the symbol table */
  inout = inout->rsibling;
  portname = inout->lchild;
  if(portname){
    while(portname){
      /* if the input is not in the symbol table already add it, 
	 else this is a semantic error */
      if(!lookup_symnode(symtab, portname->value.string_val)){
	snode = create_symnode(portname->value.string_val, RETURN);
	insert_symnode(symtab, snode);
	dpnode = agnode(dp, snode->identifier, TRUE);
	dprec = (datapath_node) agbindrec(dpnode, "datapath_node",
					  sizeof(struct datapath_node),
					  FALSE);
	dprec->operator = RETURN;
      } else {
	printf("symbol %s has been used\n", portname->value.string_val);
	errors += 1;
      }
      portname = portname->rsibling;
    }
  }

  return errors;
}

/* 
   handles binary arithmetic operators by creating setting the
   operator on the datapath_node passed through data,
   creating datapath nodes for the children,
   and recursing
 */
static int binary_arithmetic_op(astnode node, void *data){
  int errors = 0;

  /* cast the input node and create the nodes for the operands */
  Agnode_t 
    *dpnode = (Agnode_t *) data,
    *dpop1 = agnode(current_dp, NULL, TRUE),
    *dpop2 = agnode(current_dp, NULL, TRUE);

  /* set the label for the node */
  agsafeset(dpnode, "label", dpname(node), "");

  /* attach the record we are using for this node */
  datapath_node dprec = (datapath_node) agbindrec(dpnode, "datapath_node",
				    sizeof(struct datapath_node),
				    FALSE);

  /* set the operator type on the record */
  dprec->operator = node->type;

  /* recurse to left and hook into the datapath */
  handlenode(node->lchild, (void *) dpop1);
  agedge(current_dp, dpop1, dpnode, NULL, TRUE);

  /* recurse to the right and hook into datapath */
  handlenode(node->lchild->rsibling, (void *) dpop2);
  agedge(current_dp, dpop2, dpnode, NULL, TRUE);

  /* if we can, use their values */
  if(node->lchild->type == INTEGER && 
     node->lchild->rsibling->type == INTEGER){
    int l1 = 0, l2 = 0;
    l1 = d64toi(((datapath_node) 
		 aggetrec(dpop1, "datapath_node", FALSE))->value);
    l1 = d64toi(((datapath_node) 
		 aggetrec(dpop2, "datapath_node", FALSE))->value);
    if(node->type == ADDOP){
      dprec->value = l1 + l2;
    }
    if(node->type == SUBOP){
      dprec->value = l1 - l2;
    }
    if(node->type == MULOP){
      dprec->value = l1 * l2;
    }
  }

  return errors;
}


/* 
   handles an integer literal, encoding its value in Dec64
   and putting that value in the datapath node passed in
 */
static int integer_literal(astnode node, void *data){
  int errors = 0;
  Agnode_t *dpnode = (Agnode_t *) data;

  /* set the label for the node */
  agsafeset(dpnode, "label", dpname(node), "");

  /* set the node's record and the record's value */
  datapath_node dprec = (datapath_node) agbindrec(dpnode, "datapath_node",
				    sizeof(struct datapath_node),
				    FALSE);
  dprec->value = itod64(node->value.integer_val);

  return errors;
}

/* 
   handles assignment to a non subscripted variable
   after recursing down the expression
   to connect it into the datapath via the identifier
 */
static int bindvar_ident_up(astnode node, void *data){
  int errors = 0;
  Agnode_t 
    *expnode = (Agnode_t *) data, 
    *idnode = agnode(current_dp, node->lchild->value.string_val, TRUE);
  agedge(current_dp, expnode, idnode, NULL, TRUE);

  return errors;
}

/* 
   ensures that the module has a name and assigns at least one output
*/
static int program_up(astnode node, void *data){
  int errors = 0,
    returns = 0;
  symnode symbol = current_symbols(symtab);
  GVC_t *gvc;
  FILE *file = fopen("datapath.dot", "w");
  
  /* ensure there is a at least one output value */
  while(symbol){
    if(symbol->type == RETURN){
      returns++;
    }
    symbol = symbol->next;
  }
  if(!returns){
    printf("program must output at least one value\n");
    errors++;
  }

  /* output the datapath */
  agwrite(datapath, file);
  printf("the following datapath was written to datapath.dot:\n");
  agwrite(datapath, stdout);
  fclose(file);

  return errors;
}

/* 
   handles each node of the abstract sytax tree
   calls itself recursively
 */
int handlenode(astnode node, void *data){
  int errors = 0;

  /* big switch to delegate to helpers */
  switch(node->type){
  case PROGRAM:
    errors += program_down(node, data);
    errors += handlenode(node->lchild, data);
    errors += program_up(node, data);
    break;
  case STATEMENT:
    errors += handlenode(node->lchild, data);
    break;
  case IDENTIFIER:
    break;
  case INTEGER:
    errors += integer_literal(node, data);
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
    if(node->lchild->type == IDENTIFIER){
      /* make datapath node for root of expression, and recurse */
      Agnode_t *dpnode = agnode(current_dp, NULL, TRUE);
      errors += handlenode(node->lchild->rsibling, (void *) dpnode);
      errors += bindvar_ident_up(node, (void *) dpnode);
    } else if(node->lchild->type == SUBSCRIPT) {
      
    }
    break;
  case BINDFUN:
    errors += bindfun_down(node, data);
    errors += handlenode(node->lchild->rsibling->rsibling->rsibling, data);
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
  case EADDOP:
  case SUBOP:
  case ESUBOP:
  case MULOP:
  case EMULOP:
  case DIVOP:
  case EDIVOP:
  case MODOP:
  case EMODOP:
  case EXPOP:
  case EEXPOP:
    errors += binary_arithmetic_op(node, data);

    break;
  case NEGOP:
    break;
  case ENEGOP:
    break;
  case NOTOP:
    break;
  default:
    break;
  }

  return errors;
}
/* 
   main entry point for code generation
   traverses the AST with a recursive switch
   all the while sharing the symboltable
*/
void codegen(astnode root){
  int errors = 0;
  handlenode(root, NULL);
  print_symbols(symtab);
  printf("%d semantic errors\n", errors);
}
