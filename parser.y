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
;

bindvar:
identifier ASSIGN expression
{
  astnode node = create_astnode(BINDVAR);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
;

bindfun:
identifier LPAREN formal-params RPAREN ASSIGN expression
{
  astnode node = create_astnode(BINDFUN), n;
  node->lchild = $1;
  node->lchild->rsibling = $3;
  n = node->lchild;
  while(n->rsibling) 
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

index:
LBRACKET identifier RBRACKET
{
  if($2->type != IDENTIFIER){
    yyerror("syntax error: only identifiers are allowed in range index declarations");
    return -1;
  }

  astnode node = create_astnode(INDEX);
  node->lchild = $2;
  $$ = node;
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
