#include <stdio.h>

extern char *final; // the final source code after compilation
void compile(char *); // the compiler



char * invoke_compile(char *what){
  if(what == NULL){
    printf("DEBUG> not compiling from a NULL pointer");
    return final;
  }
  compile(what);
  return final;
}
