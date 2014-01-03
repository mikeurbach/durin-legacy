#include "repl.h"

void repl(void){
  int input;
  unsigned char buffer[MAX_LINE];
  int idx = 0;

  /* loop till EOF or error */
  printf(">>> ");
  do {
    /* read a character */
    input = getchar();
    
    /* if the user pressed enter */
    if(input == '\n'){
      printf(">>> ");

      /* do something with buffer */

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
	buffer[idx++] = (unsigned char) input;
      }
    }
  } while(input != EOF);
  printf("\n");
}
