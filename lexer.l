%{
#include <string.h>
#include "ast.h"
#include "parser.tab.h"

int lexLookup(char *);
%}

white [ \t]+
letter [A-Za-z]
digit [0-9]
bool "true"|"false"
id ({letter}|_)({letter}|{digit}|_)*
integer {digit}+
float {integer}"."{integer}

%x string
%x comment

%option noyywrap

%%

{white}          /* skip spaces and tabs */
"\n"             ;

{bool}           return BOOLCONST;
{id}             return lexLookup(yytext);
{integer}        return INTEGERCONST;
{float}          return FLOATCONST;


\"           { BEGIN(string); }
<string>\\\" { yymore(); }
<string>\"   {
  BEGIN(INITIAL);		/* go back to normal state */
  yytext[--yyleng] = '\0';	/* remove the trailing double-quote */
  return STRINGCONST;
 }
<string>.    { yymore(); }

"/*"		BEGIN(comment);
<comment>"*/"	BEGIN(INITIAL);
<comment>.	;

"::"         return TYPEANNOT;
"("          return LPAREN;
")"          return RPAREN;
","          return COMMA;
"="          return ASSIGN;

"+"          return PLUS;
"-"          return MINUS;
"*"          return TIMES;
"/"          return DIVIDE;
"%"          return REMAINDER;
"^"          return EXP;
".+"         return EPLUS;
".-"         return EMINUS;
".*"         return ETIMES;
"./"         return EDIVIDE;
".%"         return EREMAINDER;
".^"         return EEXP;

"=="         return ISEQUAL;
"!="         return NOTEQUAL;
"<"          return LESS;
">"          return GREATER;
"<="         return LESSEQ;
">="         return GREATEREQ;
"!"          return NOT;
"&&"         return AND;
"||"         return OR;

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
      { NULL, ID }
    };

  combo *kw;
  keywords[KWCOUNT-1].keyword = s;
  for (kw = keywords; strcmp(kw->keyword, s) != 0; kw++)
    ;
  return kw->token;
}
