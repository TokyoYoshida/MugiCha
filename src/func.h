#ifndef FUNC_H
#define FUNC_H 1

#include "support.h"
#include "symbol.h"
#include "type.h"
#include "dynode.h"

#define FUNC_MAX 100

typedef struct _FUNC {
  SYMBOL *sym;
  ASTNODE *def;
  ASTNODE *body;
  TYPE type;
  ASTNODE *def_args;
} FUNC;

typedef struct _FUNCRESULT {
  void *retvalue;
} FUNCRESULT;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void set_func_pos(int newpos);
int get_func_pos();
FUNC *lookup_func(SYMBOL *s);
FUNC *make_func(SYMBOL *s, TYPE type, ASTNODE *def, ASTNODE *body, ASTNODE *def_args);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
