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
%token COMMA

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
statement
{
  root = $1;
  $$ = root;
}
;

statement:
identifier ASSIGN expression
{
  astnode node = create_astnode(BINDVAR);
  node->lchild = $1;
  node->lchild->rsibling = $3;
  $$ = node;
}
|
identifier LPAREN identifier-list RPAREN ASSIGN expression
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

identifier-list:
identifier COMMA identifier-list
{
  if($3)
    $1->rsibling = $3;
  else {
    yyerror("syntax error: dangling comma in identifier list");
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

  //yydebug = 1;
  
  yy_scan_bytes(buffer, len);
  retval = yyparse();

  return retval ? NULL : root;
}

int yyerror(char *s){
  fprintf(stderr, "%s\n", s);
  return -1;
}
