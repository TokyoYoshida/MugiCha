#include <string.h>
#include "symbol.h"
#include "support.h"

static SYMBOL syms[SYMBOL_MAX];
static int pos = 0;

SYMBOL *make_symbol(char *name)
{
  SYMBOL *s;

  s = lookup_symbol(name);
  if(!s) ASSERT_FAIL("memory error");

  s->name = name;

  return s;
}

SYMBOL *lookup_symbol(char *name)
{
  int i;
  SYMBOL *s;

  for( i = 0 ; i < pos ; i++ ){
    if( !strcmp(syms[i].name, name)) return &syms[i];
  }

  if(pos == SYMBOL_MAX) ASSERT_FAIL("out of bounds.");

  syms[pos].name = strdup(name);

  return &syms[pos++];

}

char *symbol_description( SYMBOL *s)
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
