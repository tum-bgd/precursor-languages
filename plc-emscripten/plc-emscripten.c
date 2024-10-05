#include <stdio.h>

extern char *final; // the final source code after compilation
void compile(char *, int); // the compiler



char * invoke_compile(char *what, int ll){
  printf("invoking with language level %d ",ll);
  if(what == NULL){
    printf("DEBUG> not compiling from a NULL pointer");
    return final;
  }
  compile(what, ll);
  return final;
}
