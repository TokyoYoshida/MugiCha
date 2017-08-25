#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mugicha.h"
#include "support.h"
#include "func.h"

ASTNODE *make_astnode(){
  ASTNODE *p;
  p = (ASTNODE *)malloc(sizeof(ASTNODE));
  if(!p) ASSERT_FAIL("memory error");

  p->type.kind =  ANY;
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
  np->val.type.kind =  INT;
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
  np->val.type.kind =  DOUBLE;
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
  np->val.type.kind =  STRING;
  np->val.val.s = val;
  np->left  = NULL;
  np->right = NULL;

  return np;
}

ASTNODE *make_ast_bool(BOOL val)
{
  ASTNODE *np;
  int *vp;

  np = make_astnode();

  np->op    = VALUEDATA;
  np->val.type.kind =  BOOLTYPE;
  np->val.val.b = val;
  np->left  = NULL;
  np->right = NULL;

  return np;
}


ASTNODE *make_ast_op(OPERATION op, ASTNODE *lhr, ASTNODE *rhr)
{
    ASTNODE *np;


    np = make_astnode();

    np->type.kind =  ANY;
    np->op   = op;
    np->left = lhr;
    np->right = rhr;


    return np;
}

ASTNODE *make_ast_get_var(char *name)
{
    ASTNODE *np;

TMP_DEBUGL;

    np = make_astnode();
    np->type.kind =  ANY;// TODO ここで型情報を設定しておくとあとで型の不整合エラーをチェックできるかもしれない
    np->op         = GET_VAR;
    np->sym        = lookup_symbol(name);
    np->left       = NULL;
    np->right      = NULL;

    return np;
}

ASTNODE *make_ast_cmd(OPERATION op, ASTNODE *argp)
{
    ASTNODE *np;

    np = make_astnode();

    np->type.kind =  ANY;
    np->op   = op;
    np->left = argp;
    np->right = NULL;

    return np;
}

ASTNODE *make_ast_if(ASTNODE *cond, ASTNODE *then_stmt, ASTNODE *else_stmt)
{
    ASTNODE *np;

    np = make_astnode();

    np->type.kind =  ANY;
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

    np->type.kind =  ANY;
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
    np->type.kind =  ANY;
    np->op         = CALL_FUNC;
    np->sym        = lookup_make_symbol(name);
    f = lookup_func(np->sym);
    np->set_args       = set_args;

    // printf("call func symbol = %s",symbol_description(np->sym));

    // print_astnodeln(0, np);


    return np;
}

ASTNODE *make_ast_call_method(char *reviever_name, char *method_name, ASTNODE *set_args)
{
    ASTNODE *np;
    FUNC *f;

    np = make_astnode();
    np->type.kind =  ANY;
    np->op         = CALL_METHOD;
    np->reciever   = lookup_make_symbol(reviever_name);
    np->sym        = lookup_make_symbol(method_name);
    f = lookup_func(np->sym);
    np->set_args       = set_args;

    return np;
}

ASTNODE *make_ast_def_func(char *name, ASTNODE *def_args, char *type_name, ASTNODE *body)
{
    ASTNODE *np;


    np = make_astnode();

    np->type.kind =  get_type_by_name(type_name);
    np->op         = DEF_FUNC;
    np->sym        = lookup_make_symbol(name);
    np->left       = NULL;
    np->right      = NULL;

    make_func(np->sym, np->type, np, body, def_args);

    return np;
}

ASTNODE *make_ast_def_method(char *reciever_name, char *method_name, ASTNODE *def_args, char *type_name, ASTNODE *body)
{
  ASTNODE *np;

  np = make_astnode();

  np->reciever_type.kind = KLASS;
  np->reciever_type.klass = lookup_make_symbol(reciever_name);
  np->type.kind =  get_type_by_name(type_name);
  np->op         = DEF_METHOD;
  np->sym        = lookup_make_symbol(method_name);
  np->left       = NULL;
  np->right      = NULL;

  make_method(np->reciever_type, np->sym, np->type, np, body, def_args);

  return np;
}

ASTNODE *make_ast_def_class(char *name, ASTNODE *def_vars, ASTNODE *def_funcs)
{
  ASTNODE *np;


  np = make_astnode();

  np->type.kind =  ANY;
  np->op         = DEF_CLASS;
  np->sym        = lookup_make_symbol(name);
  np->def_vars   = def_vars;
  np->def_funcs  = def_funcs;

  return np;
}


ASTNODE *make_ast_def_var(char *name, char *type_name)
{
    ASTNODE *np;


    np = make_astnode();

    np->type.kind =  get_type_by_name(type_name);
    if(np->type.kind == KLASS) np->type.klass = lookup_make_symbol(type_name);
    np->op         = DEF_VAR;
    np->sym        = lookup_make_symbol(name);
    np->left       = NULL;
    np->right      = NULL;

    return np;
}

ASTNODE *make_ast_set_var(char *name, ASTNODE *newval)
{
    ASTNODE *np;
    SYMBOL *s;

    s = lookup_symbol(name);

    np = make_astnode();

    np->type.kind =  ANY;
    np->op         = SET_VAR;
    np->sym        = s;
    np->left       = newval;
    np->right      = NULL;

    // printf("symbol = %s",symbol_description(np->sym));

    // print_astnodeln(0, np);

    return np;
}

ASTNODE *make_ast_set_member_var(char *var_name, char *member_name, ASTNODE *newval)
{
    ASTNODE *np;
    SYMBOL *s, *mems;

TMP_DEBUGL;

    s = lookup_symbol(var_name);
    mems = lookup_symbol(member_name);

    np = make_astnode();

    np->type.kind =  ANY;
    np->op         = SET_MEMBER_VAR;
    np->sym        = s;
    np->member     = mems;
    np->left       = newval;
    np->right      = NULL;

    // printf("symbol = %s",symbol_description(np->sym));

    // print_astnodeln(0, np);

    return np;
}

ASTNODE *make_ast_get_member_var(char *var_name, char *member_name)
{
    ASTNODE *np;
    SYMBOL *s, *mems;

TMP_DEBUGL;

    s = lookup_symbol(var_name);
    mems = lookup_symbol(member_name);

    np = make_astnode();

    np->type.kind =  ANY;
    np->op         = GET_MEMBER_VAR;
    np->sym        = s;
    np->member     = mems;
    np->left       = NULL;
    np->right      = NULL;

    // printf("symbol = %s",symbol_description(np->sym));

    // print_astnodeln(0, np);

    return np;
}

void print_astnode(int depth, ASTNODE *np)
{
  int i;
  char *t, *o ,*v;

  for(i = 0 ; i < depth ; i++){
    printf(" ");
  }

  t = get_type_description(np->type.kind);
  o = get_op_description(np->op);
  v = value_description(np->val);
  printf("ast type.kind =  %s oper = %s val = %s ",t ,o, v);



  if(np->op == DEF_VAR){
    printf("/ val symbol : %s / ",symbol_description(np->sym));
  }
  if(np->op == DEF_FUNC){
    printf("/ func symbol : %s / ",symbol_description(np->sym));
  }

/*
  printf("addr:%p type.kind =  %s oper = %s val = %s left = %p right = %p\n",
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
