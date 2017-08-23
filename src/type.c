#include <stdio.h>
#include <string.h>
#include "type.h"
#include "support.h"


char *value_description(VALUE val)
{
  static char ret[STRING_LENGTH_MAX];

  switch(val.type.kind){
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

char *get_type_description(TYPEKIND i)
{
  static char *type_a[] = {"ANY", "INT", "DOUBLE","BOOLTYPE","STRING","KLASS"};

  if(i > STRING ) ASSERT_FAIL("out of bounds");

  return type_a[i];
}

char *get_op_description(OPERATION i)
{
  static char *op_a[]   = {"NONE", "VALUEDATA", "ADD", "SUB", "MUL", "DIV", "SEQ",
    "PRINTDATA", "DEF_VAR", "SET_VAR", "GET_VAR", "DEF_FUNC", "DEF_METHOD", "CALL_FUNC","CMP_EQ", "CMP_NOTEQ", "CMP_GREATER", "CMP_SMALLE", "CMP_GREATEREQ", "CMP_SMALLEEQ", "IF_STMT","WHILE_STMT","SET_MEMBER_VAR", "GET_MEMBER_VAR", "DEF_CLASS"};

  if(i > DEF_CLASS) ASSERT_FAIL("out of bounds");

  return op_a[i];
}

TYPEKIND get_type_by_name(char *name)
{
  if(!strcmp(name,"int")){
    return INT;
  } else if (!strcmp(name,"double")){
    return DOUBLE;
  } else if (!strcmp(name,"bool")){
    return BOOLTYPE;
  } else if (!strcmp(name,"string")){
    return STRING;
  } else if (lookup_symbol(name)){
    return KLASS;
  }
  ASSERT_FAIL("this block expect never call.");
}

char *get_bool_description(BOOL val)
{
  static char *bools[] = {"false","true"};
  return bools[val];
}
