#include "durin.h"

int main(int argc, char *argv[]){
  int status = 0;
  struct stat buff;

  /* no arguments, just go to REPL */
  if(argc == 1){
    repl();
  } 
  /* 1 argument */
  else if(argc == 2){
    /* check if the argument is a valid file */
    if(stat(argv[1], &buff) == -1){
      /* it isn't valid */
      status = -1;
      printf("Could not open file %s\n", argv[1]);
    } else {
      /* it is */

      script(argv[1]);
    }
  }

  return status;
}
