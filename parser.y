%{
  #include <stdio.h>
  #include "ast.h"

  #define YYDEBUG 1

  astnode root, id;

  extern int yylex();
  extern char *yytext;

  int yyerror(char *s);
%}

%token SYMBOL INTEGER DECIMAL SCIENTIFIC NULL TRUE FALSE LPAREN RPAREN LBRACKET RBRACKET LCURLY RCURLY SEMICOLON COMMA

%right BIND

%right ARROW

%left QUESTION COLON

%left OR
%left AND
%left ISEQUAL NOTEQUAL
%left LESS GREATER LESSEQ GREATEREQ
%left NOT

%left PLUS MINUS
%left TIMES DIVIDE REMAINDER
%right EXP
%left DOT

%%

program: statement-list;

statement-list: statement-list SEMICOLON statement | statement ;
statement: enumeration SEMICOLON | binding SEMICOLON ;

enumeration: SYMBOL LESS symbol-list GREATER LCURLY statement-list RCURLY ;
symbol-list: SYMBOL COMMA symbol-list | SYMBOL ;

binding: value BIND expression ;
expression: application | lambda | math | ternary | value | literal ;

application: SYMBOL LPAREN actual-args RPAREN ;
actual-args: expression COMMA actual-args | expression ;

lambda: LPAREN formal-args RPAREN ARROW LCURLY statement-list expression RCURLY | LPAREN formal-args RPAREN ARROW expression ;
formal-args: formal-value COMMA formal-args | formal-value ;
formal-value: SYMBOL formal-dimensions | SYMBOL ;
formal-dimensions: LBRACKET SYMBOL RBRACKET formal-dimensions | LBRACKET SYMBOL RBRACKET ;

math: expression binary-op expression | expression DOT binary-op expression ;
binary-op: PLUS | MINUS | TIMES | DIVIDE | REMAINDER | EXP ;

ternary: boolean-expression QUESTION expression COLON expression ;
boolean-expression: boolean-expression AND boolean-expression | boolean-expression OR boolean-expression | NOT boolean-expression | expression compare-op expression
compare-op: GREATER | GREATEREQ | LESS | LESSEQ | ISEQUAL | NOTEQUAL ;

value: SYMBOL actual-dimensions | SYMBOL ;
actual-dimensions: LBRACKET expression RBRACKET actual-dimensions | LBRACKET expression RBRACKET ;
literal: NULL | TRUE | FALSE | INTEGER | DECIMAL | SCIENTIFIC | matrix ;
matrix: LBRACKET literal-list RBRACKET ;
literal-list: literal COMMA literal-list | literal ;

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
