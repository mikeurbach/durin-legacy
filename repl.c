#include "repl.h"
#include "ast.h"

extern astnode parse_buffer(char *, int);

void repl(void){
  int input;
  char buffer[MAX_LINE];
  int idx = 0;
  astnode ast_root;

  /* loop till EOF or error */
  printf(">>> ");
  do {
    /* read a character */
    input = getchar();
    
    /* if the user pressed enter */
    if(input == '\n'){
      /* parse buffer */
      ast_root = parse_buffer(buffer, idx);

      /* print the ast, unless there was a syntax error */
      if(ast_root)
	print_ast(ast_root, 0);

      printf(">>> ");

      /* reset the buffer and index, and go on to next line */
      memset(buffer, 0, MAX_LINE);
      idx = 0;
      continue;
    } 
    /* if it's any other character */
    else {
      /* if the buffer is full */
      if(idx == MAX_LINE){
	printf("REPL input is limited to %d characters\n", MAX_LINE);
	memset(buffer, 0, MAX_LINE);
	idx = 0;
      }
      /* otherwise we are good to go */
      else {
	buffer[idx++] = (char) input;
      }
    }
  } while(input != EOF);
  printf("\n");
}
