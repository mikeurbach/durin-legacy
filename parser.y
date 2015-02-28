%{
  #include <stdio.h>
  #include "ast.h"

  #define YYDEBUG 1

  astnode root, id;

  extern int yylex();
  extern char *yytext;

  int yyerror(char *s);
%}

%token SYMBOL INTEGER DECIMAL SCIENTIFIC NULL TRUE FALSE LPAREN RPAREN LBRACKET RBRACKET LCURLY RCURLY COMMA DOTS SEMICOLON DIMS ARROW

%right BIND

%left QUESTION COLON
%left NOT UMINUS EUMINUS

%left OR
%left AND
%left ISEQUAL NOTEQUAL
%left LESS GREATER LESSEQ GREATEREQ

%left DOT
%left PLUS MINUS
%left TIMES DIVIDE REMAINDER
%left EXP

%%

program: statement-list {};
statement-list: statement-list statement {} | statement {};
statement: enumeration SEMICOLON {} | binding SEMICOLON {};
enumeration: SYMBOL LESS symbol-list GREATER LCURLY statement-list RCURLY {};
binding: formal-value BIND expression {};
expression: application {} | lambda {} | math {} | ternary {} | value {} | literal {};
application: SYMBOL LPAREN actual-args RPAREN {};
actual-args: expression COMMA actual-args {} | expression {};
lambda: formal-args ARROW LCURLY statement-list expression RCURLY {} | formal-args ARROW expression {};
formal-args: formal-value COMMA formal-args {} | formal-value {};
formal-value: SYMBOL LBRACKET symbol-list RBRACKET {} | SYMBOL {};
math: expression binary-op expression {} | expression DOT binary-op expression {};
binary-op: PLUS {} | MINUS {} | TIMES {} | DIVIDE {} | REMAINDER {} | EXP {};
ternary: boolean-expression QUESTION expression COLON expression {};
boolean-expression: boolean-expression AND boolean-expression {} | boolean-expression OR boolean-expression {} | NOT boolean-expression | expression compare-op expression
compare-op: GREATER {} | GREATEREQ {} | LESS {} | LESSEQ {} | ISEQUAL {} | NOTEQUAL {};
value: SYMBOL {} | SYMBOL LBRACKET expression-list RBRACKET {};
expression-list: expression COMMA expression {} | expression {};
symbol-list: SYMBOL COMMA symbol-list {} | SYMBOL {};
literal-list: literal COMMA literal-list {} | literal {};
literal: NULL {} | TRUE {} | FALSE {} | INTEGER {} | DECIMAL {} | SCIENTIFIC {} | matrix {};
matrix: LBRACKET literal-list RBRACKET {};

%%

astnode parse_buffer(char *buffer, int len){
  int retval;
  extern int yydebug;

  yydebug = 0;
  
  yy_scan_bytes(buffer, len);
  retval = yyparse();

  return retval ? NULL : root;
}

int yyerror(char *s){
  fprintf(stderr, "%s\n", s);
  return -1;
}
