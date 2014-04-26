#include "ast.h"

astnode create_astnode(int tokentype){
  astnode node = malloc(sizeof(struct astnode));
  memset(node, 0, sizeof(struct astnode));
  node->type = tokentype;
  return node;
}

int destroy_ast(astnode node){
  int ret = 0;
  if(node){
    ret += destroy_ast(node->lchild);
    ret += destroy_ast(node->rsibling);

    if(node->type == STRING){
      free(node->value.string_val);
    }
    if(node->type == BOOL){
      free(node->value.bool_val);
    }

    free(node);
    ret += 1;
  }

  return ret;
}

int traverse_ast(astnode node, int (*fn)(astnode, void*), void *data){
  int count = 0;
  if(node){
    count += traverse_ast(node->lchild, fn, data);
    count += fn(node, data);
    count += traverse_ast(node->rsibling, fn, data);
  }
  return count;
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
    printf("%s)", root->value.bool_val);
    leaf = true;
  }

  if(root->type == RANGE){
    printf("[%d...%d])", root->lower_bound, root->upper_bound);
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
