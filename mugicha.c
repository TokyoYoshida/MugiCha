#include <stdio.h>
#include <stdlib.h>
#include "mugicha.h"
#include "stack.h"
#include "support.h"
#include "var.h"

void cleanup_astnodes(ASTNODE *np)
{
    STACK ns;

    stack_init(&ns);

    stack_push(&ns, np);

    while(!stack_is_empty(&ns)){
      np = (ASTNODE *)stack_pop(&ns);

      if(np->left){
        stack_push(&ns, np->left);
      }
      if(np->right){
        stack_push(&ns, np->right);
      }

    }
  }

int astn_has_sub(ASTNODE *p)
{
  return p->left || p->right;
}

int dyn_has_sub(DYNODE *p)
{
  return p->left || p->right || p->set_args || p->def_args;
}

VALUE do_interpret(ASTNODE *ast_rootp)
{
  VALUE v;

  v = eval_node(ast_rootp);

  return v;
}

void mugicha_main(ASTNODE *rootp)
{
  VALUE dp;

  printf("\n------\nAST is \n");
  print_ast(0, rootp);
  printf("------\nstart interpret .. \n");

  dp = do_interpret(rootp);

  printf("\nresult is %s\n" ,value_description(dp));
}
