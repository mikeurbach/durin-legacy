%{
  #include <stdio.h>
  #include "ast.h"

  #define YYDEBUG 1

  astnode root, id;

  extern int yylex();
  extern char *yytext;

  int yyerror(char *s);
%}

%token ID INTEGERCONST FLOATCONST STRINGCONST BOOLCONST LPAREN RPAREN LBRACKET RBRACKET LCURLY RCURLY COMMA DOTS SEMICOLON DIMS

%right ASSIGN

%left QUESTION COLON
%left NOT UMINUS EUMINUS

%left OR
%left AND
%left ISEQUAL NOTEQUAL
%left LESS GREATER LESSEQ GREATEREQ

%left PLUS MINUS EPLUS EMINUS
%left TIMES DIVIDE REMAINDER ETIMES EDIVIDE EREMAINDER
%left EXP EEXP

%%

program: 
bindfun
{
  astnode node = create_astnode(PROGRAM);
  node->lchild = $1;
  $$ = root = node;
}
;

statement-list:
statement statement-list
{
  astnode node = create_astnode(STATEMENT);
  node->lchild = $1;
  node->rsibling = $2;

  $$ = node;
}
|
{
  $$ = NULL;
}
;

statement:
bindvar SEMICOLON
{
  $$ = $1;
}
|
bindfun
{
  $$ = $1;
}
|
function-application SEMICOLON
{
  $$ = $1;
}
|
indexed-block
{
  $$ = $1;
}
;

bindvar:
identifier ASSIGN expression
{
  astnode node = create_astnode(BINDVAR);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
|
subscript ASSIGN expression
{
  astnode node = create_astnode(BINDVAR);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
;

bindfun:
LBRACKET identifier-list RBRACKET ASSIGN identifier LPAREN identifier-list RPAREN LCURLY statement-list RCURLY
{
  astnode 
    bind = create_astnode(BINDFUN),
    params = create_astnode(PARAMS),
    ret = create_astnode(RETURN);
  
  if(!$10){
    yyerror("syntax error: cannot define a function without a body");
    return -1;
  }

  bind->lchild = $5;

  params->lchild = $7;
  bind->lchild->rsibling = params;

  ret->lchild = $2;
  bind->lchild->rsibling->rsibling = ret;

  bind->lchild->rsibling->rsibling->rsibling = $10;

  $$ = bind;
}
;

indexed-block:
identifier index-list LCURLY statement-list RCURLY
{
  astnode node = create_astnode(INDEXEDBLOCK), n;
  node->lchild = $1;
  n = node->lchild->rsibling = $2;
  while(n->rsibling)
    n = n->rsibling;
  n->rsibling = $4;
  $$ = node;
}
;

expression:
expression PLUS expression
{
  astnode node = create_astnode(ADDOP);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
|
expression MINUS expression
{
  astnode node = create_astnode(SUBOP);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
|
expression TIMES expression
{
  astnode node = create_astnode(MULOP);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
|
expression DIVIDE expression
{
  astnode node = create_astnode(DIVOP);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
|
expression REMAINDER expression
{
  astnode node = create_astnode(MODOP);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
|
expression EXP expression
{
  astnode node = create_astnode(EXPOP);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
|
MINUS expression %prec UMINUS
{
  astnode node = create_astnode(NEGOP);
  node->lchild = $2;
  $$ = node;
}
|
LPAREN expression RPAREN
{
  $$ = $2;
}
|
function-application
{
  $$ = $1;
}
|
ternary-operator
{
  $$ = $1;
}
|
range
{
  $$ = $1;
}
|
identifier
{
  $$ = $1;
}
|
subscript
{
  $$ = $1;
}
|
literal
{
  $$ = $1;
}
;

boolean-expression:
boolean-expression OR boolean-expression
{
  astnode node = create_astnode(OROP);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
|
boolean-expression AND boolean-expression
{
  astnode node = create_astnode(ANDOP);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
|
NOT boolean-expression
{
  astnode node = create_astnode(NOTOP);
  node->lchild = $2;
  $$ = node;
}
|
expression ISEQUAL expression
{
  astnode node = create_astnode(EQOP);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
|
expression NOTEQUAL expression
{
  astnode node = create_astnode(INEQOP);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
|
expression LESS expression
{
  astnode node = create_astnode(LTOP);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
|
expression LESSEQ expression
{
  astnode node = create_astnode(LTEOP);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
|
expression GREATER expression
{
  astnode node = create_astnode(GTOP);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
|
expression GREATEREQ expression
{
  astnode node = create_astnode(GTEOP);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
|
BOOLCONST
{
  astnode node = create_astnode(BOOL);
  node->value.bool_val = strdup(yytext);
  $$ = node;
}
;

ternary-operator:
boolean-expression QUESTION expression COLON expression
{
  astnode node = create_astnode(TERNARYOP);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  node->lchild->rsibling->rsibling = $5;
  $$ = node;
}
;

function-application:
identifier LPAREN expression-list RPAREN
{
  astnode node = create_astnode(FUNCALL);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
;

expression-list:
expression COMMA expression-list
{
  if($3)
    $1->rsibling = $3;
  else {
    yyerror("syntax error: extra comma in actual params");
    return -1;
  }
  $$ = $1;
}
|
expression
{
  $$ = $1;
}
|
{
  $$ = NULL;
}
;

identifier-list:
sized-identifier COMMA identifier-list
{
  if($3)
    $1->rsibling = $3;
  else {
    yyerror("syntax error: extra comma in identifier list");
    return -1;
  }
  $$ = $1;
}
|
sized-identifier
{
  $$ = $1;
}
|
{
  $$ = NULL;
}
;

range:
LBRACKET literal DOTS literal RBRACKET
{
  if($2->type != INTEGER || $4->type != INTEGER){
    yyerror("syntax error: only Integer literals are allowed in range definitions");
    return -1;
  }

  astnode node = create_astnode(RANGE);
  node->lower_bound = $2->value.integer_val;
  node->upper_bound = $4->value.integer_val;
  $$ = node;
}
;

index-list:
index index-list
{
  $1->rsibling = $2;
  $$ = $1;
}
|
index
{
  $$ = $1;
}
;

index:
LESS identifier GREATER
{
  astnode node = create_astnode(INDEX);
  node->lchild = $2;
  $$ = node;
}
;

index-expr-list:
index-expr index-expr-list
{
  $1->rsibling = $2;
  $$ = $1;
}
|
index-expr
{
  $$ = $1;
}
;

index-expr:
LBRACKET expression RBRACKET
{
  astnode node = create_astnode(INDEX);
  node->lchild = $2;
  $$ = node;
}
;

subscript:
identifier index-expr-list
{
  astnode node  = create_astnode(SUBSCRIPT);
  node->lchild = $1;
  node->lchild->rsibling = $2;
  $$ = node;
}
;

sized-identifier:
identifier COLON COLON size
{
  $1->lchild = $4;
  $$ = $1;
}
|
identifier
{
  astnode node = create_astnode(INTEGER);
  node->value.integer_val = 1;
  $1->lchild = node;
  $$ = $1;
}
;

size:
literal COLON size
{
  if($1->type != INTEGER){
    yyerror("syntax error: only Integer literals are allowed in matrix size definitions");
    return -1;
  }
  if($1->value.integer_val < 1){
    yyerror("syntax error: a size must be a positive");
    return -1;
  }
  $1->rsibling = $3;
  $$ = $1;
}
|
literal
{
  if($1->type != INTEGER){
    yyerror("syntax error: only Integer literals are allowed in matrix size definitions");
    return -1;
  }
  if($1->value.integer_val < 1){
    yyerror("syntax error: a size must be a positive");
    return -1;
  }
  $$ = $1;
}
|
{
  $$ = NULL;
}
;

identifier:
ID
{
  astnode node = create_astnode(IDENTIFIER);
  node->value.string_val = strdup(yytext);
  $$ = node;
}
;

literal:
INTEGERCONST
{
  astnode node = create_astnode(INTEGER);
  node->value.integer_val = atoi(yytext);
  $$ = node;
}
|
FLOATCONST
{
  astnode node = create_astnode(FLOAT);
  node->value.float_val = atof(yytext);
  $$ = node;
}
;


%%

astnode parse_buffer(char *buffer, int len){
  int retval;
  extern int yydebug;

  yydebug = 1;
  
  yy_scan_bytes(buffer, len);
  retval = yyparse();

  return retval ? NULL : root;
}

int yyerror(char *s){
  fprintf(stderr, "%s\n", s);
  return -1;
}
