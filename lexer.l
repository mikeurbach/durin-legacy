%{
#include <string.h>
#include "ast.h"
#include "parser.tab.h"

int lexLookup(char *);
%}

white [ \t\n]+

null "null"
true "true"
false "false"
digit [0-9]
nonzero [1-9]
integer {nonzero}*{digit}
decimal {integer}"."{digit}+
scientific {decimal}"e""-"?{integer}
letter [a-zA-Z]
symbol {letter}({letter}|{digit}|"_"|"-"|"'")*

%option noyywrap

%%

{white}          /* skip whitespace */

{null}       return NULL;
{true}       return TRUE;
{false}      return FALSE;
{integer}    return INTEGER;
{decimal}    return DECIMAL;
{scientific} return SCIENTIFIC;
{symbol}     return lexLookup(yytext);

":"          return COLON;
"?"          return QUESTION;
"("          return LPAREN;
")"          return RPAREN;
"["          return LBRACKET;
"]"          return RBRACKET;
"{"          return LCURLY;
"}"          return RCURLY;
","          return COMMA;
"..."        return DOTS;
";"          return SEMICOLON;
"="          return BIND;
"->"         return ARROW;

"+"          return PLUS;
"-"          return MINUS;
"*"          return TIMES;
"/"          return DIVIDE;
"%"          return REMAINDER;
"^"          return EXP;
"."          return DOT;

"!"          return NOT;
"&&"         return AND;
"||"         return OR;

"=="         return ISEQUAL;
"!="         return NOTEQUAL;
"<"          return LESS;
">"          return GREATER;
"<="         return LESSEQ;
">="         return GREATEREQ;

%%

int lexLookup(char *s) {
  struct combo {
    char *keyword;
    int token;
  };
  typedef struct combo combo;

  const int KWCOUNT = 1;

  static combo keywords[] =
    { 
      { NULL, SYMBOL }
    };

  combo *kw;
  keywords[KWCOUNT-1].keyword = s;
  for (kw = keywords; strcmp(kw->keyword, s) != 0; kw++)
    ;
  return kw->token;
}
