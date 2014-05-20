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

int traverse_ast_up(astnode node, int (*fn)(astnode, void*), void *data){
  int count = 0;
  if(node){
    count += traverse_ast_up(node->lchild, fn, data);
    count += fn(node, data);
    count += traverse_ast_up(node->rsibling, fn, data);
  }
  return count;
}

int traverse_ast_down(astnode node, int (*fn)(astnode, void*), void *data){
  int count = 0;
  if(node){
    count += fn(node, data);
    count += traverse_ast_down(node->lchild, fn, data);
    count += traverse_ast_down(node->rsibling, fn, data);
  }
  return count;
}

static int print_ast_help(astnode node, int depth){
  static int count = 1;
  astnode child;
  int i;
  bool leaf = false;

  if(depth == 0)
    count = 1;
  else
    count++;

  for(i = 0; i < depth; i++) 
    printf("  ");

  printf("(%s ", token_table[node->type].token);

  if(node->type == INTEGER){
    printf("%d)", node->value.integer_val);
    leaf = true;
  }

  if(node->type == FLOAT){
    printf("%f)", node->value.float_val);
    leaf = true;
  }

  if(node->type == STRING || node->type == IDENTIFIER){
    printf("%s)", node->value.string_val);
    leaf = true;
  }

  if(node->type == BOOL){
    printf("%s)", node->value.bool_val);
    leaf = true;
  }

  if(node->type == RANGE){
    printf("[%d...%d])", node->lower_bound, node->upper_bound);
    leaf = true;
  }

  printf("\n");

  for(child = node->lchild; child != NULL; child = child->rsibling)
    print_ast_help(child, depth+1);

  if(!leaf){
    for(i = 0; i < depth; i++) 
      printf("  ");
    printf(")\n");
  }

  return count;
}

int print_ast(astnode root){
  int count;
  count = print_ast_help(root, 0);
  printf("%d nodes\n", count);
  return count;
}
