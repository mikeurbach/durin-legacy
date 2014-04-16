%{
  #include <stdio.h>
  #include "ast.h"
  
  #define YYDEBUG 1

  astnode root, id;

  extern int yylex();
  extern char *yytext;

  int yyerror(char *s);
%}

%token NEWLINE

%token ID

%token INTEGERCONST
%token FLOATCONST
%token STRINGCONST
%token BOOLCONST

%token TYPEANNOT
%token LPAREN
%token RPAREN
%token LBRACKET
%token RBRACKET
%token LCURLY
%token RCURLY
%token LANGLE
%token RANGLE
%token COMMA
%token DOTS
%token SEMICOLON

%right ASSIGN

%left OR
%left AND
%left ISEQUAL NOTEQUAL
%left LESS GREATER LESSEQ GREATEREQ
%left PLUS MINUS EPLUS EMINUS
%left TIMES DIVIDE REMAINDER ETIMES EDIVIDE EREMAINDER
%left EXP EEXP
%left NOT UMINUS EUMINUS

%%

program: 
statement-list
{
  astnode node = create_astnode(PROGRAM);
  node->lchild = $1;
  $$ = root = node;
}
;

statement-list:
statement SEMICOLON statement-list
{
  astnode node = create_astnode(STATEMENT);
  node->lchild = $1;
  if($3)
    node->rsibling = $3;

  $$ = node;
}
|
{
  $$ = NULL;
}
;

statement:
bindvar
{
  $$ = $1;
}
|
bindfun
{
  $$ = $1;
}
|
function-application
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
indexed-expr-identifier ASSIGN expression
{
  astnode node = create_astnode(BINDVAR);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
;

bindfun:
LBRACKET return-list RBRACKET ASSIGN identifier LPAREN formal-params RPAREN LCURLY statement-list RCURLY
{
  astnode 
    bind = create_astnode(BINDFUN),
    params = create_astnode(PARAMS),
    ret = create_astnode(RETURN),
    n;
  
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
identifier LESS index-list GREATER  LCURLY statement-list RCURLY
{
  astnode node = create_astnode(INDEXEDBLOCK), n;
  node->lchild = $1;
  node->lchild->rsibling = $3;
  n = node->lchild->rsibling;
  while(n->rsibling != NULL)
    n = n->rsibling;
  n->rsibling = $6;
  
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
indexed-expr-identifier
{
  $$ = $1;
}
|
literal
{
  $$ = $1;
}
;

formal-params:
formal-param COMMA formal-params
{
  if($3)
    $1->rsibling = $3;
  else {
    yyerror("syntax error: extra comma in formal params");
    return -1;
  }
  $$ = $1;
}
|
formal-param
|
{
  $$ = NULL;
}
;

formal-param:
identifier
|
index
;

function-application:
identifier LPAREN actual-params RPAREN
{
  astnode node = create_astnode(FUNCALL);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
;

actual-params:
actual-param COMMA actual-params
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
actual-param
|
{
  $$ = NULL;
}
;

actual-param:
expression
{
  $$ = $1;
}
;

return-list:
identifier COMMA return-list
{
  if($3)
    $1->rsibling = $3;
  else {
    yyerror("syntax error: extra comma in return list");
    return -1;
  }
  $$ = $1;
}
|
identifier
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
LBRACKET identifier RBRACKET
{
  if($2->type != IDENTIFIER){
    yyerror("syntax error: only identifiers are allowed in indices");
    return -1;
  }

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

indexed-expr-identifier:
identifier index-expr-list
{
  $1->lchild = $2;
  $$ = $1;
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
