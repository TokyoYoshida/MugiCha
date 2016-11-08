#include <string.h>
#include "type.h"
#include "support.h"


char *value_description(VALUE val)
{
  static char ret[STRING_LENGTH_MAX];

  switch(val.type){
    case INT:
    sprintf(ret, "%d" ,val.val.i);
      break;
    case DOUBLE:
    sprintf(ret, "%f" , val.val.d);
      break;
    case BOOLTYPE:
    sprintf(ret, "%s" , get_bool_description(val.val.b));
    break;
    case STRING:
    sprintf(ret, "%s" ,val.val.s);
    break;
    case ANY:
      return "ANY";
  }


  return ret;
}

char *get_type_description(TYPE i)
{
  static char *type_a[] = {"ANY", "INT", "DOUBLE","BOOLTYPE","STRING"};

  if(i > STRING ) ASSERT_FAIL("out of bounds");

  return type_a[i];
}

char *get_op_description(OPERATION i)
{
  static char *op_a[]   = {"NONE", "VALUEDATA", "ADD", "SUB", "MUL", "DIV", "SEQ",
    "PRINTDATA", "DEF_VAL", "SET_VAL", "GET_VAL","DEF_FUNC", "CALL_FUNC","CMP_EQ",
    "CMP_NOTEQ", "CMP_GREATER", "CMP_SMALLE", "CMP_GREATEREQ", "CMP_SMALLEEQ",
    "IF_STMT","WHILE_STMT"};

  if(i > WHILE_STMT) ASSERT_FAIL("out of bounds");

  return op_a[i];
}

TYPE get_type_by_name(char *name)
{
  if(!strcmp(name,"int")){
    return INT;
  } else if (!strcmp(name,"double")){
    return DOUBLE;
  } else if (!strcmp(name,"bool")){
    return BOOLTYPE;
  } else if (!strcmp(name,"string")){
    return STRING;
  }
  ASSERT_FAIL("this block expect never call.");
}

char *get_bool_description(BOOL val)
{
  static char *bools[] = {"false","true"};
  return bools[val];
}

int cast_int(VALUE v)
{
  switch(v.type){
    case INT:
      return (int )v.val.i;
    case DOUBLE:
      return (int )v.val.d;
    case BOOLTYPE:
      return (int )v.val.b;
    case STRING:
    case ANY:
      ASSERT_FAIL_BLOCK();
  }
}

double cast_double(VALUE v)
{
  switch(v.type){
    case INT:
      return (double )v.val.i;
    case DOUBLE:
      return (double )v.val.d;
    case BOOLTYPE:
      return (double )v.val.b;
    case STRING:
    case ANY:
      ASSERT_FAIL_BLOCK();
  }
}

BOOL cast_bool(VALUE v)
{
  switch(v.type){
    case INT:
      return (BOOL )v.val.i;
    case DOUBLE:
      return (BOOL )v.val.d;
    case BOOLTYPE:
      return (BOOL )v.val.b;
    case STRING:
    case ANY:
      ASSERT_FAIL_BLOCK();
  }
}

int comp_val(VALUE lhs, VALUE rhs)
{
  switch(lhs.type)
  {
    case INT:
      if(lhs.val.i > rhs.val.i )
        return 1;
      else if(lhs.val.i < rhs.val.i){
        return -1;
      } else {
        return 0;
      }
    case DOUBLE:
      if(lhs.val.d > rhs.val.d )
        return 1;
      else if(lhs.val.d < rhs.val.d){
        return -1;
      } else {
        return 0;
      }
    case BOOLTYPE:
      if(rhs.type != BOOLTYPE) ASSERT_FAIL("type error.");

      if(lhs.val.b == rhs.val.b )
        return 0;
      else {
        return 1;
      }
    case STRING:
      if(rhs.type != STRING) ASSERT_FAIL("type error.");

      return strcmp(lhs.val.s, rhs.val.s);
    case ANY:
      ASSERT_FAIL("this block expect never call,");
  }

  ASSERT_FAIL("this block expect never call,");
}
