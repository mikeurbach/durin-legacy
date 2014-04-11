#include "ast.h"

astnode create_astnode(int tokentype){
  astnode node = malloc(sizeof(struct astnode));
  memset(node, 0, sizeof(struct astnode));
  node->type = tokentype;
  return node;
}

void print_ast(astnode root, int depth){
  astnode child;
  int i;
  bool leaf = false;

  for(i = 0; i < depth; i++) 
    printf("  ");

  printf("(%s ", token_table[root->type].token);

  if(root->type == INTEGER){
    printf("%d)", root->value.integer_val);
    leaf = true;
  }

  if(root->type == FLOAT){
    printf("%f)", root->value.float_val);
    leaf = true;
  }

  if(root->type == STRING || root->type == IDENTIFIER){
    printf("%s)", root->value.string_val);
    leaf = true;
  }

  if(root->type == BOOL){
    printf("%d)", root->value.bool_val);
    leaf = true;
  }

  printf("\n");

  for(child = root->lchild; child != NULL; child = child->rsibling)
    print_ast(child, depth+1);

  if(!leaf){
    for(i = 0; i < depth; i++) 
      printf("  ");
    printf(")\n");
  }
}
