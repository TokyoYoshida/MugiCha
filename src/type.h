#ifndef TYPE_H
#define TYPE_H 1

typedef enum _BOOL {FALSE, TRUE} BOOL;

typedef enum _TYPE {ANY, INT, DOUBLE, BOOLTYPE,STRING} TYPE;
typedef enum _OPERATION {NONE, VALUEDATA, ADD, SUB, MUL, DIV, SEQ, PRINTDATA,
    DEF_VAL, SET_VAL, GET_VAL, DEF_FUNC, CALL_FUNC, CMP_EQ, CMP_NOTEQ,
    CMP_GREATER, CMP_SMALLER, CMP_GREATEREQ, CMP_SMALLEREQ ,IF_STMT,WHILE_STMT } OPERATION ;

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
char *get_type_description(TYPE i);
char *get_op_description(OPERATION i);
TYPE get_type_by_name(char *name);
char *get_bool_description(BOOL val);
int comp_val(VALUE lhr, VALUE rhr);
double cast_double(VALUE v);
int cast_int(VALUE v);
BOOL cast_bool(VALUE v);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
