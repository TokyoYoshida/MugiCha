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

ASTNODE *root_call_main_inserted(ASTNODE *rootp)
{
  ASTNODE *call_main;

  call_main = make_ast_call_func("main" ,NULL);

  return make_ast_op(SEQ, rootp, call_main);
}

void mugicha_main(ASTNODE *rootp)
{
  ASTNODE *root_call_main;
  VALUE dp;

  root_call_main = root_call_main_inserted(rootp);

  printf("\n------\nAST is \n");
  print_ast(0, root_call_main);
  printf("------\nstart interpret .. \n");

  dp = do_interpret(root_call_main);

  printf("\nresult is %s\n" ,value_description(dp));
}
