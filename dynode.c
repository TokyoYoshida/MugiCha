#include <stdlib.h>
#include "mugicha.h"
#include "support.h"
#include "var.h"
#include "eval.h"

void cleanup_dynode(DYNODE *p)
{
DEBUGS("◼◼◼◼◼◼◼");
DEBUGP(p);
  p->op  = NONE;
  p->type = ANY;
  p->val = NULL;
  p->left = NULL;
  p->right = NULL;
  p->def_args = NULL;
  p->set_args = NULL;
  p->evaled = FALSE;
}

DYNODE *make_dynode(ASTNODE *ast)
{
  DYNODE *dp;

  dp = (DYNODE *)malloc(sizeof(DYNODE));
  cleanup_dynode(dp);

  dp->ast = ast;

  return dp;
}

void make_dynode_sub(DYNODE *dp)
{
  if(dp->ast->left){
    dp->left = (DYNODE *)malloc(sizeof(DYNODE));
    if( !dp->left ) ASSERT_FAIL("memory error");
    dp->left->ast = dp->ast->left;
  }

  if(dp->ast->right){
    dp->right = (DYNODE *)malloc(sizeof(DYNODE));
    if( !dp->right ) ASSERT_FAIL("memory error");
    dp->right->ast = dp->ast->right;
  }

  return;
}

void print_depth(int depth)
{
  int i;
  for(i = 0 ; i < depth ; i++){
    printf(" ");
  }
}

void print_dynode(int depth, DYNODE *np)
{
  VAR *v;

  if(!np) return;

  print_depth(depth);

  printf("dy evaled = %s, type = %s oper = %s val = %s / ",
                                        get_bool_description(np->evaled),
                                        get_type_description(np->type),
                                        get_op_description(np->op),
                                        value_description(np->type, np->val)
                                      );
  print_astnodeln(0, np->ast);
  if(np->op == DEF_VAL){
    printf(" / symbol : %s / ",symbol_description(np->sym));
    v = lookup_var(np->sym);
    if(v) printf(" / variable : %s",variable_description(v));

  }
  if(np->ast && np->ast->op == GET_VAL){
    printf(" get target val name = %s / ",np->ast->sym->name);
  }
  if(np->def_args){
    printf("/ def_args ");
    print_dynode(depth + 1 , np->def_args);
  }
  if(np->set_args){
    printf("/ set_args ");
    print_dynode(depth + 1 , np->set_args);
  }
  printf("\n");

/*
  printf("addr:%p type = %s oper = %s val = %s left = %p right = %p\n",
                                        np ,
                                        type_a[np->type],
                                        op_a[np->op],
                                        value_description(np->type, np->val),
                                        np->left,
                                        np->right
                                      );
*/
}

void print_dynodetree(int depth, DYNODE *np)
{
  print_dynode(depth, np);
  if(np->left){
    printf("left    ");
    print_dynode(depth + 1 ,np->left);
  }
  if(np->right){
    printf("right   ");
    print_dynode(depth + 1 ,np->right);
  }
  if(np->def_args){
    printf("def args");
    print_dynode(depth + 1 ,np->def_args);
  }
  if(np->set_args){
    printf("set args");
    print_dynode(depth + 1 ,np->set_args);
  }
}
