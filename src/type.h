#ifndef TYPE_H
#define TYPE_H 1

#include "symbol.h"

typedef enum _BOOL {FALSE, TRUE} BOOL;

typedef enum _TYPEKIND {ANY, INT, DOUBLE, BOOLTYPE, STRING, ARRAY, KLASS} TYPEKIND;

typedef struct _TYPE {
  TYPEKIND kind;
  TYPEKIND elem_kind;
  SYMBOL *klass;
} TYPE;

typedef enum _OPERATION {NONE, VALUEDATA, ADD, SUB, MUL, DIV, SEQ, EXPR_LIST, DEF_ARGS, PRINTDATA, DEF_VAR, SET_VAR, GET_VAR, DEF_FUNC, DEF_METHOD, CALL_FUNC, CALL_METHOD, CMP_EQ, CMP_NOTEQ,CMP_GREATER, CMP_SMALLER, CMP_GREATEREQ, CMP_SMALLEREQ ,IF_STMT,WHILE_STMT ,SET_MEMBER_VAR, SET_ARRAY_VAR, GET_MEMBER_VAR, GET_ARRAY_VAR, NEW_CLASS, DEF_CLASS } OPERATION ; // if modify this place, you also must modify function `get_op_description`.

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
