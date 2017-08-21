#ifndef SYMBOL_H
#define SYMBOL_H 1

#define SYMBOL_MAX 100

typedef struct _SYMBOL {
  char *name;
} SYMBOL;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

SYMBOL *lookup_make_symbol(char const *name);
SYMBOL *make_symbol( char const *name);
SYMBOL *lookup_symbol(char const *name);
char *symbol_description(SYMBOL *s);
void print_symbols();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
