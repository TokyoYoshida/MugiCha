#include <stdio.h>
#include <string.h>
#include "support.h"
#include "var.h"

static VAR vars[VAR_MAX];
static int pos = 0;

void set_var_pos(int newpos)
{
  pos = newpos;
}

int get_var_pos()
{
  return pos;
}

VAR *lookup_var(SYMBOL *s)
{
  int i;

  for(i = pos-1 ; i >= 0 ; i--){
    if( vars[i].sym == s ) return &vars[i];
  }
  return NULL;
}

VAR *make_var(SYMBOL *s, TYPE type)
{
  VAR *v;

  if( pos == VAR_MAX) ASSERT_FAIL("out of bounds.");

  vars[pos].sym = s;
  vars[pos].data.type = type;
  vars[pos].data.val.v = NULL;

  return &vars[pos++];
}

void set_var_by_symbol(TYPE type, SYMBOL *s ,VALUE newval)
{
  VAR *v;

  v = lookup_var(s);
  if(!v) ASSERT_FAIL("null pointer.");

  v->data = newval;
}

char *variable_description(VAR *v)
{
  static char ret[100];

  if(!v) ASSERT_FAIL("null pointer.");

  sprintf(ret, " var value = %s " ,value_description(v->data));

  return ret;
}

void print_variables()
{
  int i;
  VAR *v;

  printf(" variable count = %d \n", pos);
  for(i = 0 ; i < pos ; i++ ){
    v = &vars[i];
    printf(" variable no = %d , name = %s, val = %s ,type = %s \n", i, v->sym->name ,variable_description(v), get_type_description(v->data.type));
  }
}
