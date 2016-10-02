#include "func.h"

static FUNC funcs[FUNC_MAX];
static int pos;

void set_func_pos(int newpos)
{
  pos = newpos;
}

int get_func_pos()
{
  return pos;
}


FUNC *lookup_func(SYMBOL *s)
{
  int i;

  for(i = pos-1 ; i >= 0 ; i--){
    if( funcs[i].sym == s ) return &funcs[i];
  }

  return NULL;
}

FUNC *make_func(SYMBOL *s, TYPE type, ASTNODE *def, ASTNODE *body, ASTNODE *def_args)
{
  if( pos == FUNC_MAX) ASSERT_FAIL("out of bounds.");

  funcs[pos].sym = s;
  funcs[pos].type = type;
  funcs[pos].def = def;
  funcs[pos].body = body;
  funcs[pos].def_args = def_args;

  return &funcs[pos++];
}
