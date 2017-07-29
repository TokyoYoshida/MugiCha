#include <stdio.h>
#include <stdlib.h>
#include "mugicha.h"
#include "stack.h"
#include "support.h"


void display_ast(ASTNODE *rootp)
{
  ASTNODE *root_call_main;

  root_call_main = root_call_main_inserted(rootp);

  printf("\n------\nAST is \n");
  print_ast(0, root_call_main);
  printf("------\nstart interpret .. \n");
}
