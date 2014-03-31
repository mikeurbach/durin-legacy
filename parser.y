%{
  #include <stdio.h>
  #include "ast.h"
  
  #define YYDEBUG 1

  astnode root;

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
expression
{
  root = $1;
  $$ = root;
}
;

expression:
literal PLUS literal
{
  astnode node = create_astnode(ADDOP);
  node->lchild = $1;
  node->lchild->rsibling = $3;
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

%%

int main(int argc, char *argv[]){
  extern int yydebug;
  yydebug = 1;

  yyparse();

  print_ast(root, 0);

  return 0;
}

int yyerror(char *s){
  fprintf(stderr, "%s\n", s);
  return 0;
}
