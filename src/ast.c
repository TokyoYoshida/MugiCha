#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mugicha.h"
#include "support.h"
#include "var.h"
#include "func.h"

ASTNODE *make_astnode(){
  ASTNODE *p;
  p = (ASTNODE *)malloc(sizeof(ASTNODE));
  if(!p) ASSERT_FAIL("memory error");

  p->type = ANY;
  p->op = NONE;
  p->val.val.v = NULL;
  p->sym = NULL;
  p->left = NULL;
  p->right = NULL;
  p->set_args = NULL;
  p->condition = NULL;

  return p;
}

ASTNODE *make_ast_int(int val)
{
  ASTNODE *np;
  int *vp;

  np = make_astnode();

  np->op    = VALUEDATA;
  np->val.type  = INT;
  np->val.val.i = val;
  np->left  = NULL;
  np->right = NULL;

  return np;
}

ASTNODE *make_ast_double(double val)
{
  ASTNODE *np;
  int *vp;

  np = make_astnode();

  np->op    = VALUEDATA;
  np->val.type  = DOUBLE;
  np->val.val.d = val;
  np->left  = NULL;
  np->right = NULL;

  return np;
}

ASTNODE *make_ast_string(char *val)
{
  ASTNODE *np;
  int *vp;

  np = make_astnode();

  np->op    = VALUEDATA;
  np->val.type  = STRING;
  np->val.val.s = val;
  np->left  = NULL;
  np->right = NULL;

  return np;
}

ASTNODE *make_ast_bool(BOOL val)
{
  ASTNODE *np;
  int *vp;

  np->op    = VALUEDATA;
  np->val.type  = BOOLTYPE;
  np->val.val.b = val;
  np->left  = NULL;
  np->right = NULL;

  return np;
}


ASTNODE *make_ast_op(OPERATION op, ASTNODE *lhr, ASTNODE *rhr)
{
    ASTNODE *np;


    np = make_astnode();

    np->type = ANY;
    np->op   = op;
    np->left = lhr;
    np->right = rhr;


    return np;
}

ASTNODE *make_ast_get_var(char *name)
{
    ASTNODE *np;

    np = make_astnode();

    np->type       = ANY;
    np->op         = GET_VAL;
    np->sym        = lookup_symbol(name);
    np->left       = NULL;
    np->right      = NULL;

    return np;
}

ASTNODE *make_ast_cmd(OPERATION op, ASTNODE *argp)
{
    ASTNODE *np;

    np = make_astnode();

    np->type = ANY;
    np->op   = op;
    np->left = argp;
    np->right = NULL;

    return np;
}

ASTNODE *make_ast_if(ASTNODE *cond, ASTNODE *then_stmt, ASTNODE *else_stmt)
{
    ASTNODE *np;

    np = make_astnode();

    np->type       = ANY;
    np->op         = IF_STMT;
    np->condition   = cond;
    np->left       = then_stmt;
    np->right      = else_stmt;

    return np;
}

ASTNODE *make_ast_while(ASTNODE *cond, ASTNODE *loop_stmt)
{
    ASTNODE *np;

    np = make_astnode();

    np->type       = ANY;
    np->op         = WHILE_STMT;
    np->condition   = cond;
    np->left       = loop_stmt;
    np->right      = NULL;

    return np;
}


ASTNODE *make_ast_call_func(char *name, ASTNODE *set_args)
{
    ASTNODE *np;
    FUNC *f;

    np = make_astnode();

    np->type       = ANY;
    np->op         = CALL_FUNC;
    np->sym        = lookup_symbol(name);
    f = lookup_func(np->sym);
    np->set_args       = set_args;

    printf("call func symbol = %s",symbol_description(np->sym));

    print_astnodeln(0, np);


    return np;
}

ASTNODE *make_ast_def_func(char *name, ASTNODE *def_args, TYPE type, ASTNODE *body)
{
    ASTNODE *np;


    np = make_astnode();

    np->type       = type;
    np->op         = DEF_FUNC;
    np->sym        = make_symbol(name);
    np->left       = NULL;
    np->right      = NULL;

    make_func(np->sym, type, np, body, def_args);

    printf("symbol = %s",symbol_description(np->sym));

    print_astnodeln(0, np);



    return np;
}

ASTNODE *make_ast_def_var(char *name, TYPE type)
{
    ASTNODE *np;


    np = make_astnode();

    np->type       = type;
    np->op         = DEF_VAL;
    np->sym        = make_symbol(name);
    np->left       = NULL;
    np->right      = NULL;

    printf("symbol = %s",symbol_description(np->sym));

    print_astnodeln(0, np);

    return np;
}

ASTNODE *make_ast_set_var(char *name, ASTNODE *newval)
{
    ASTNODE *np;
    SYMBOL *s;


    s = lookup_symbol(name);

    np = make_astnode();

    np->type       = ANY;
    np->op         = SET_VAL;
    np->sym        = s;
    np->left       = newval;
    np->right      = NULL;

    printf("symbol = %s",symbol_description(np->sym));

    print_astnodeln(0, np);

    return np;
}

void print_astnode(int depth, ASTNODE *np)
{
  int i;
  char *t, *o ,*v;

  for(i = 0 ; i < depth ; i++){
    printf(" ");
  }



  t = get_type_description(np->type);
  o = get_op_description(np->op);
  v = value_description(np->val);
  printf("ast type = %s oper = %s val = %s ",t ,o, v);



  if(np->op == DEF_VAL){
    printf("/ val symbol : %s / ",symbol_description(np->sym));
  }
  if(np->op == DEF_FUNC){
    printf("/ func symbol : %s / ",symbol_description(np->sym));
  }

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
void print_astnodeln(int depth, ASTNODE *np)
{
  print_astnode(depth, np);
  printf("\n");
}


void print_ast(int depth, ASTNODE *np)
{
  print_astnodeln(depth, np);

  if(np->left)
    print_ast(depth + 1, np->left);
  if(np->right)
    print_ast(depth + 1, np->right);
}
