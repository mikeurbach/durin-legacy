#include "symtab.h"

static int hashPJW(char *s){
  unsigned h = 0, g;
  char *p;

  for (p = s; *p != '\0'; p++) {
    h = (h << 4) + *p;
    if ((g = (h & 0xf0000000)) != 0)
      h ^= (g >> 24) ^ g;
  }

  return h % SYMTAB_SIZE;
}

symnode create_symnode(char *identifier, int type){
  symnode node = malloc(sizeof(struct symnode));
  memset(node, 0, sizeof(struct symnode));
  node->identifier = strdup(identifier);
  node->type = type;
  return node;
}

static void destroy_symnode(symnode node){
  free(node->identifier);
  if(node->type == BINDVAR)
    free(node->dims);
  else if(node->type == BINDFUN){
    free(node->params);
    free(node->returns);
  }
  free(node);
}

static symhashtable create_symhashtable(){
  symhashtable hashtable = malloc(sizeof(struct symhashtable));
  hashtable->table = calloc(SYMTAB_SIZE, sizeof(symnode));
  memset(hashtable->table, 0, SYMTAB_SIZE * sizeof(symnode));
  hashtable->outer_scope = NULL;
  hashtable->level = -1;
  return hashtable;
}

static void destroy_symhashtable(symhashtable hashtable){
  int i;
  for(i = 0; i < SYMTAB_SIZE; i++){
    symnode node, next;
    for(node = hashtable->table[i]; node != NULL; node = next){
      next = node->next;
      destroy_symnode(node);
    }
  }
}

static symnode lookup_in_symhashtable(symhashtable hashtable, char *identifier){
  symnode node = NULL;
  int slot = hashPJW(identifier);
  for(node = hashtable->table[slot]; 
      node != NULL && strcmp(node->identifier, identifier) != 0;
      node = node->next)
    ;
  return node;
}

static symnode insert_into_symhashtable(symhashtable hashtable, symnode node){
  int slot = hashPJW(node->identifier);
  symnode existing = lookup_in_symhashtable(hashtable, node->identifier);
  if(existing != NULL)
    return existing;
  node->next = hashtable->table[slot];
  hashtable->table[slot] = node;
  return node;
}

symboltable create_symboltable(){
  symboltable symtab = malloc(sizeof(struct symboltable));
  memset(symtab, 0, sizeof(struct symboltable));
  symtab->inner_scope = create_symhashtable();
  symtab->inner_scope->level = 0;
  return symtab;
}

void destroy_symboltable(symboltable symtab){
  symhashtable hashtable, outer;
  for(hashtable = symtab->inner_scope; hashtable != NULL; hashtable = outer){
    outer = hashtable->outer_scope;
    destroy_symhashtable(hashtable);
  }
}

symnode lookup_symnode(symboltable symtab, char *identifier){
  symnode node;
  symhashtable hashtable;

  for(node = NULL, hashtable = symtab->inner_scope;
      node == NULL && hashtable != NULL;
      hashtable = hashtable->outer_scope){
    node = lookup_in_symhashtable(hashtable, identifier);
  }

  return node;
}

symnode insert_symnode(symboltable symtab, symnode node){
  symnode existing = lookup_symnode(symtab, node->identifier), new;
  if(existing != NULL)
    return existing;
  new = insert_into_symhashtable(symtab->inner_scope, node);
  return new;
}

void enter_scope(symboltable symtab){
  symhashtable hashtable = create_symhashtable();
  hashtable->outer_scope = symtab->inner_scope;
  hashtable->level = symtab->inner_scope->level + 1;
  symtab->inner_scope = hashtable;
}

void leave_scope(symboltable symtab){
  symhashtable hashtable = symtab->inner_scope;
  symtab->inner_scope = hashtable->outer_scope;
  destroy_symhashtable(hashtable);
}
