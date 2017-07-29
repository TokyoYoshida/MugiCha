#include <stdio.h>
#include <stdlib.h>
#include "mugicha.h"
#include "stack.h"
#include "support.h"


void display_ast(ASTNODE *rootp)
{
  printf("\n------\nAST is \n");
  print_ast(0, rootp);
  printf("------\nstart interpret .. \n");
}
