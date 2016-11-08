#include <string.h>
#include "support.h"
#include "mugicha.h"
#include "eval.h"
#include "var.h"
#include "func.h"
#include "env.h"

VALUE exec_print(ASTNODE *ap)
{
  VALUE ret;

  ret = eval_node(ap->left);
  printf("print value = %s (%s) !!", value_description(ret), get_type_description(ret.type));
  printf("\n");

  return ret;
}

VALUE exec_cmp(ASTNODE *ap ,OPERATION comp_mode)
{
  int r;
  BOOL res;
  VALUE ret, lhs ,rhs;

  lhs = eval_node(ap->left);
  rhs = eval_node(ap->right);

  r = comp_val(lhs, rhs);
  switch(comp_mode){
    case CMP_EQ:
      res = (r == 0);
      break;
    case CMP_NOTEQ:
      res = !(r == 0);
      break;
    case CMP_GREATER:
      res = (r == 1);
      break;
    case CMP_SMALLER:
      res = (r == -1);
      break;
    case CMP_GREATEREQ:
      res = (r == 1) || (r == 0);
      break;
    case CMP_SMALLEREQ:
      res = (r == -1) || (r == 0);
      break;
    default:
      ASSERT_FAIL("invalid operation.");
  }

  ret.type = BOOLTYPE;
  ret.val.b = res;

  return ret;
}

double calc_double(VALUE lhs, VALUE rhs,OPERATION calc_mode)
{
  switch(calc_mode){
    case ADD:
    return cast_double(lhs) + cast_double(rhs);
    case SUB:
    return cast_double(lhs) - cast_double(rhs);
    case MUL:
    return cast_double(lhs) * cast_double(rhs);
    case DIV:
    return cast_double(lhs) / cast_double(rhs);
    default:
    ASSERT_FAIL_BLOCK();
    }
}

int calc_int(VALUE lhs, VALUE rhs,OPERATION calc_mode)
{
  switch(calc_mode){
    case ADD:
    return cast_int(lhs) + cast_int(rhs);
    case SUB:
    return cast_int(lhs) - cast_int(rhs);
    case MUL:
    return cast_int(lhs) * cast_int(rhs);
    case DIV:
    return cast_int(lhs) / cast_int(rhs);
    default:
    ASSERT_FAIL_BLOCK();
    }
}

VALUE exec_calc(ASTNODE *ap,OPERATION calc_mode)
{
  VALUE ret;
  VALUE lhs ,rhs;
  char *s;
  char *rhs_str;

  lhs = eval_node(ap->left);
  rhs = eval_node(ap->right);

  switch(lhs.type){
    case INT:
    ret.type = INT;
    ret.val.i = calc_int(lhs, rhs, calc_mode);
    return ret;
    case DOUBLE:
    ret.type = DOUBLE;
    ret.val.d = calc_double(lhs, rhs, calc_mode);
    return ret;
    case STRING:
      if(calc_mode == ADD){
        ret.type = STRING;
        if( rhs.type == STRING)
          rhs_str = rhs.val.s;
        else {
          rhs_str = value_description(rhs);
        }
        s = malloc(strlen(lhs.val.s) + strlen(rhs_str) + 1);
        if(!s) ASSERT_FAIL_MEMORY();
        strcpy(s, lhs.val.s);
        strcat(s, rhs_str);
        ret.val.s = s;
        return ret;
      } else {
        ASSERT_FAIL_BLOCK();
      }
    case BOOLTYPE:
    case ANY:
      ASSERT_FAIL_BLOCK();
  }
}

VALUE  exec_seq(ASTNODE *ap)
{
  VALUE lhs ,rhs;

  lhs = eval_node(ap->left);
  rhs = eval_node(ap->right);

  return rhs;
}

VALUE exec_def_func(ASTNODE *ap)
{
  VALUE ret;

  ret.type = BOOLTYPE;
  ret.val.b = TRUE;

  return ret;
}

VALUE exec_call_func(ASTNODE *ap)
{
  VALUE ret, lhs ,rhs, def_args, set_args;
  FUNC *f;

  push_env();

  f = lookup_func(ap->sym);

  if( ap->set_args != NULL){
    eval_node(f->def_args);
    eval_node(ap->set_args);
  }
  ret = eval_node(f->body);

  pop_env();

  return ret;
}

VALUE exec_if(ASTNODE *ap)
{
  VALUE cond_res, ret;

  cond_res = eval_node(ap->condition);
  if(cond_res.val.b == TRUE)
    ret = eval_node(ap->left);
  else if( ap->right ) {
    ret = eval_node(ap->right);
  }

  return ret;
}

VALUE exec_while(ASTNODE *ap)
{
  VALUE ret;

  while(1){
    ret = eval_node(ap->condition);
    if(ret.val.b == TRUE)
      ret = eval_node(ap->left);
    else {
      return ret;
    }
  }
}

VALUE exec_def_var(ASTNODE *ap)
{
  VALUE ret;

  make_var(ap->sym, ap->type);

  ret.type = BOOLTYPE;
  ret.val.b = TRUE;

  return ret;
}

VALUE exec_set_var(ASTNODE *ap)
{
  VALUE ret;

  ret = eval_node(ap->left);

  set_var_by_symbol(ret.type, ap->sym, ret);

  return ret;
}

VALUE exec_get_var(ASTNODE *ap)
{
  VALUE ret;
  VAR *v;

  v = lookup_var(ap->sym);

  ret = v->data;

  return ret;
}


VALUE eval_node_op(ASTNODE *ap)
{
  switch(ap->op){
    case ADD:
    case SUB:
    case MUL:
    case DIV:
      return exec_calc(ap ,ap->op);
    case SEQ:
      return exec_seq(ap);
    case NONE:
    ASSERT_FAIL("this block expect never call.");
    case VALUEDATA:
    ASSERT_FAIL("this block expect never call.");
    case CMP_EQ:
    case CMP_NOTEQ:
    case CMP_GREATER:
    case CMP_SMALLER:
    case CMP_GREATEREQ:
    case CMP_SMALLEREQ:
      return exec_cmp(ap, ap->op);
    case PRINTDATA:
      return exec_print(ap);
    case DEF_VAL:
      return exec_def_var(ap);
    case SET_VAL:
      return exec_set_var(ap);
    case GET_VAL:
      return exec_get_var(ap);
    case DEF_FUNC:
    return exec_def_func(ap);
    case CALL_FUNC:
    return exec_call_func(ap);
    case IF_STMT:
    return exec_if(ap);
    case WHILE_STMT:
    return exec_while(ap);
  }
  ASSERT_FAIL("this block expect never call.");
}

VALUE eval_node_value(ASTNODE *ap)
{
  return ap->val;
}


VALUE eval_node(ASTNODE *ap)
{
  int ret;

  if(ap->op == VALUEDATA ) return eval_node_value(ap);

  return eval_node_op(ap);
}
