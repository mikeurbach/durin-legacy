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
  if(node->type == BINDVAR){
    free(node->dims);
  }
  free(node);
}

static symhashtable create_symhashtable(){
  symhashtable hashtable = malloc(sizeof(struct symhashtable));
  hashtable->table = calloc(SYMTAB_SIZE, sizeof(symnode));
  memset(hashtable->table, 0, SYMTAB_SIZE * sizeof(symnode));
  hashtable->head = hashtable->tail = NULL;
  hashtable->datapath = agopen("G", Agdirected, NULL);
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
  symnode 
    existing = lookup_in_symhashtable(hashtable, node->identifier),
    n = NULL;

  if(existing != NULL)
    return existing;

  // empty hashtable initialized with head and tail on a single node
  if(!hashtable->head && !hashtable->tail){
    hashtable->head = hashtable->tail = node;
  }

  if(hashtable->table[slot]){ // collision
    // seek n to last node in this slot's chain
    for(n = hashtable->table[slot]; n && n->next; n = n->next)
      ;
    // put node between n and its prev
    node->next = n;
    node->prev = n->prev;
    n->prev->next = node;
    n->prev = node;
  } else { // no collision
    // put node in this slot and make it the tail of the hashtable
    hashtable->table[slot] = node;
    node->next = NULL;
    hashtable->tail->next = node;
    node->prev = hashtable->tail;
    hashtable->tail = node;
  }

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

symnode current_symbols(symboltable symtab){
  return symtab->inner_scope->head;
}

Agraph_t *current_datapath(symboltable symtab){
  return symtab->inner_scope->datapath;
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

void print_symbols(symboltable symtab){
  symnode node;
  symhashtable hashtable;

  printf("first scope\n");
  for(hashtable = symtab->inner_scope;
      hashtable != NULL;
      hashtable = hashtable->outer_scope){
    node = hashtable->head;
    while(node){
      printf("%s: %s\n", node->identifier, token_table[node->type].token);
      node = node->next;
    }
    if(hashtable->outer_scope){
      printf("next scope out\n");
    }
  }
}
