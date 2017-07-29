#ifndef TYPE_H
#define TYPE_H 1

#include "symbol.h"

typedef enum _BOOL {FALSE, TRUE} BOOL;

typedef enum _TYPEKIND {ANY, INT, DOUBLE, BOOLTYPE, STRING, KLASS} TYPEKIND;

typedef struct _TYPE {
  TYPEKIND kind;
  SYMBOL *klass;
} TYPE;

typedef enum _OPERATION {NONE, VALUEDATA, ADD, SUB, MUL, DIV, SEQ, PRINTDATA,
    DEF_VAR, SET_VAR, GET_VAR, DEF_FUNC, CALL_FUNC, CMP_EQ, CMP_NOTEQ,
    CMP_GREATER, CMP_SMALLER, CMP_GREATEREQ, CMP_SMALLEREQ ,IF_STMT,WHILE_STMT ,SET_MEMBER_VAR, GET_MEMBER_VAR, DEF_CLASS } OPERATION ;

union _v {
  char *s;
  void *v;
  int i;
  double d;
  BOOL b;
};

typedef struct _VALUE {
  TYPE type;
  union _v val;
} VALUE;

#define STRING_LENGTH_MAX 255

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

char *value_description(VALUE val);
char *get_type_description(TYPEKIND i);
char *get_op_description(OPERATION i);
TYPEKIND get_type_by_name(char *name);
char *get_bool_description(BOOL val);
int comp_val(VALUE lhr, VALUE rhr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
