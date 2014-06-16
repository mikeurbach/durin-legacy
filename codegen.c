#include "codegen.h"

symboltable symtab;
Agraph_t *datapath, *current_dp;

/* declarations and helpers */
void *handlenode(astnode node, int *errors);

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

void handle_dimensions(astnode node, symnode snode){
  // count dimensions
  int i = 0;
  astnode dimnode = node->lchild;
  while(dimnode){
    snode->dim_count++;
    dimnode = dimnode->rsibling;
  }

  // allocate an array of integers
  snode->dims = malloc(sizeof(int) * snode->dim_count);

  // copy the dimensions into the array
  dimnode = node->lchild;
  while(dimnode){
    snode->dims[i] = dimnode->value.integer_val;
    dimnode = dimnode->rsibling;
    i++;
  }
}

/* 
   the functions before the big handlenode switch
   are called roughly in order top to bottom and up again to traverse AST
*/

/* 
   instantiates the global symbol table and shared datapath, then
   sets up some graph attributes for outputting the datapath
*/
static void *program_down(astnode node, int *errors){
    symtab = create_symboltable();
    datapath = current_datapath(symtab);

    agattr(datapath, AGNODE, "shape", "box");
    agattr(datapath, AGRAPH, "rankdir", "LR");

    *errors += (!symtab || !datapath) ? 1 : 0;

    return NULL;
}

/* 
   enters a new scope, puts the function name, inputs, 
   and outputs in the symbol table. also creates datapath
   nodes for the inputs and outputs
*/
static void *bindfun_down(astnode node, int *errors){
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
    *errors += 1;
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
	handle_dimensions(portname, snode);
	insert_symnode(symtab, snode);
	dpnode = agnode(dp, snode->identifier, TRUE);
	dprec = (datapath_node) agbindrec(dpnode, "datapath_node",
					  sizeof(struct datapath_node),
					  FALSE);
	dprec->operator = PARAMS;
      } else {
	printf("symbol %s has been used\n", portname->value.string_val);
	*errors += 1;
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
	handle_dimensions(portname, snode);
	insert_symnode(symtab, snode);
	dpnode = agnode(dp, snode->identifier, TRUE);
	dprec = (datapath_node) agbindrec(dpnode, "datapath_node",
					  sizeof(struct datapath_node),
					  FALSE);
	dprec->operator = RETURN;
      } else {
	printf("symbol %s has been used\n", portname->value.string_val);
	*errors += 1;
      }
      portname = portname->rsibling;
    }
  }

  return NULL;
}

/* 
   handles binary arithmetic operators by creating setting the
   operator on the datapath_node passed through data,
   creating datapath nodes for the children,
   and recursing
 */
static void *binary_arithmetic_op(astnode node, int *errors){
  /* create node for this operator, and pointers for operands */
  Agnode_t *dpnode = agnode(current_dp, dpname(node), TRUE), *dpop1, *dpop2;

  /* attach the record we are using for this node */
  datapath_node dprec = (datapath_node) agbindrec(dpnode, "datapath_node",
				    sizeof(struct datapath_node),
				    FALSE);

  /* set the operator type on the record */
  dprec->operator = node->type;

  /* recurse to left and hook into the datapath */
  dpop1 = (Agnode_t *) handlenode(node->lchild, errors);
  if(!dpop1){
    printf("unhandled node in binary expression: %s\n", dpname(node->lchild));
    *errors += 1;
  } else {
    agedge(current_dp, dpop1, dpnode, NULL, TRUE);
  }

  /* recurse to the right and hook into datapath */
  dpop2 = (Agnode_t *) handlenode(node->lchild->rsibling, errors);
  if(!dpop2){
    printf("unhandled node in binary expression: %s\n", dpname(node->lchild->rsibling));
    *errors += 1;
  } else {
    agedge(current_dp, dpop2, dpnode, NULL, TRUE);
  }

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

  return (void *) dpnode;
}


/* 
   handles an unsubscripted identifier in an expression
 */
static void *identifier(astnode node, int *errors){
  Agnode_t *dpnode = agnode(current_dp, dpname(node), FALSE);
  if(!dpnode){
    printf("identifier not bound: %s\n", dpname(node));
    *errors += 1;
  }
  return (void *) dpnode;
}

/* 
   handles an integer literal, encoding its value in Dec64
   and putting that value in the datapath node passed in
 */
static void *integer_literal(astnode node, int *errors){
  Agnode_t *dpnode = agnode(current_dp, dpname(node), TRUE);

  /* set the node's record and the record's value */
  datapath_node dprec = (datapath_node) agbindrec(dpnode, "datapath_node",
				    sizeof(struct datapath_node),
				    FALSE);
  dprec->value = itod64(node->value.integer_val);

  return (void *) dpnode;
}

/* 
   handles assignment to a non subscripted variable
   after recursing down the expression
   to connect it into the datapath via the identifier
 */
static void *bindvar_ident_up(astnode node, int *errors){
  Agnode_t *dpnode = agnode(current_dp, node->lchild->value.string_val, 
			    TRUE);
  datapath_node dprec = (datapath_node) agbindrec(dpnode, "datapath_node",
				    sizeof(struct datapath_node),
				    FALSE);
  dprec->operator = IDENTIFIER;

  return (void *) dpnode;
}

/* 
   ensures that the function assigns its outputs
*/
static void *bindfun_up(astnode node, int *errors){  
  int returns = 0;
  symnode symbol = current_symbols(symtab);
  Agnode_t *dpnode;

  /* ensure all output values are assigned */
  while(symbol){
    if(symbol->type == RETURN){
      dpnode = agnode(datapath, symbol->identifier, FALSE);
      if(dpnode && agdegree(datapath, dpnode, TRUE, FALSE)){
	returns++;
      } else {
	printf("function output %s is not assigned\n", symbol->identifier);
	*errors += 1;
      }
    }
    symbol = symbol->next;
  }

  /* leave the scope, unless this is the top level scope */
  if(symtab->inner_scope->level > 1)
    leave_scope(symtab);

  return NULL;
}


/* 
   ensures that the program assigns at least one output
*/
static void *program_up(astnode node, int *errors){
  int returns = 0;
  symnode symbol = current_symbols(symtab);
  FILE *file = fopen("datapath.dot", "w");
  Agnode_t *dpnode;

  /* ensure there is at least one output value */
  while(symbol){
    dpnode = agnode(datapath, symbol->identifier, FALSE);
    if(symbol->type == RETURN && agdegree(datapath, dpnode, TRUE, FALSE)){
      returns++;
    }
    symbol = symbol->next;
  }
  if(!returns){
    printf("program must output at least one value\n");
    *errors += 1;
  }

  /* output the datapath */
  agwrite(datapath, file);
  printf("the following datapath was written to datapath.dot:\n");
  agwrite(datapath, stdout);
  fclose(file);

  return NULL;
}

/* 
   handles each node of the abstract sytax tree
   calls itself recursively
 */
void *handlenode(astnode node, int *errors){
  Agnode_t *ret = NULL;

  /* big switch to delegate to helpers */
  switch(node->type){
  case PROGRAM:
    program_down(node, errors);
    handlenode(node->lchild, errors);
    program_up(node, errors);
    break;
  case STATEMENT:
    handlenode(node->lchild, errors);
    if(node->rsibling){
      handlenode(node->rsibling, errors);
    }
    break;
  case IDENTIFIER:
    ret = identifier(node, errors);
    break;
  case INTEGER:
    ret = integer_literal(node, errors);
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
      Agnode_t *exp, *ident;
      exp = handlenode(node->lchild->rsibling, errors);
      ident = bindvar_ident_up(node, errors);
      agedge(current_dp, exp, ident, NULL, TRUE);
    } else if(node->lchild->type == SUBSCRIPT) {
      
    }
    break;
  case BINDFUN:
    bindfun_down(node, errors);
    handlenode(node->lchild->rsibling->rsibling->rsibling, errors);
    bindfun_up(node, errors);
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
    ret = binary_arithmetic_op(node, errors);

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

  return ret;
}
/* 
   main entry point for code generation
   traverses the AST with a recursive switch
   all the while sharing the symboltable
*/
void codegen(astnode root){
  int errors = 0;
  handlenode(root, &errors);
  print_symbols(symtab);
  printf("%d semantic errors\n", errors);
}
