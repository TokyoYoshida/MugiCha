#ifndef VAR_H
#define VAR_H 1

#define VAR_MAX 200

#include "type.h"
#include "symbol.h"

typedef struct _VAR {
  SYMBOL *sym;
  VALUE data;
} VAR;

VAR *lookup_var(SYMBOL *s);
VAR *make_var(SYMBOL *s, TYPE type);
char *variable_description(VAR *v);
void set_var_by_symbol(TYPE type, SYMBOL *s ,VALUE newval);
void print_variables();

void set_var_pos(int newpos);
int get_var_pos();

#endif
