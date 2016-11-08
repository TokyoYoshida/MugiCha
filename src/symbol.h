#ifndef SYMBOL_H
#define SYMBOL_H 1

#include "type.h"

#define SYMBOL_MAX 100

typedef struct _SYMBOL {
  char *name;
} SYMBOL;

SYMBOL *make_symbol(char *name);
SYMBOL *lookup_symbol(char *name);
char *symbol_description(SYMBOL *s);
void print_symbols();

#endif
