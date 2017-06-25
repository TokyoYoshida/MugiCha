#include <string.h>
#include "symbol.h"
#include "support.h"

static SYMBOL syms[SYMBOL_MAX];
static int pos = 0;

SYMBOL *lookup_make_symbol(char *name)
{
  SYMBOL *s;
  s = lookup_symbol(name);
  if(s) return s;

  return make_symbol(name);
}


SYMBOL *make_symbol(char *name)
{
  if(pos == SYMBOL_MAX) ASSERT_FAIL("out of bounds.");

  syms[pos].name = strdup(name);

  return &syms[pos++];
}

SYMBOL *lookup_symbol(char *name)
{
  int i;
  SYMBOL *s;


  for( i = 0 ; i < pos ; i++ ){
    if( !strcmp(syms[i].name, name)) return &syms[i];
  }

  return NULL;
}

char *symbol_description(SYMBOL *s)
{
  static char ret[100];
  sprintf(ret, " symbol name = %s" ,s->name);

  return ret;
}

void print_symbols()
{
  SYMBOL *s;
  int i;

  for(i = 0 ; i < pos ; i++){
    s = &syms[i];
    printf("symbol num = %d ,%s\n", i, symbol_description(s));
  }
}
