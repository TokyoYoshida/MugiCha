%{
#include <stdio.h>
#include <string.h>
#include "y.tab.h"
#include "mugicha.h"
#include "support.h"

int
yywrap(void)
{
    return 1;
}

/* TODO delete later
void print_op(ASTNODE *np){
  if(np->op != VALUEDATA){
  printf("this is not value\n");
    return;
  }

  switch(np->type){
  case INT:
    printf(">>%d\n", *(int *)np->val);
    breakl
  case DOUBLE:
    printf(">>%f\n", *(double *)np->val);
    break;
  case ANY:
    ASSERT_FAIL("this block expect never call");
  }

}
*/

%}
%%
"+"		return '+';
"-"		return '-';
"*"		return '*';
"/"		return '/';
"("		return '(';
")"		return ')';
"{"		return '{';
"}"		return '}';
"="		return '=';
"=="	return EQUAL;
"!="	return NOTEQUAL;
">"	return '>';
"<"	return '<';
">="	return GREATEREQUAL;
"<="	return SMALLEREQUAL;
"!"	return '!';
"if" return IF;
"else" return ELSE;
"print" return PRINT;
"var"   return VAR;
"func" return FUNCTION;
"while" return WHILE;
\"[^\"]*\" {
  char temp[STRING_LENGTH_MAX];
  char *s;

  strncpy(temp, yytext, strlen(yytext));

  s = malloc(strlen(yytext));
  strncpy(s, (yytext+1), strlen(yytext)-2);
  if(!s) ASSERT_FAIL_MEMORY();

  yylval.np = make_ast_string(s);

  return STRING_LITERAL;
}
[0-9]+\.*[0-9]* {
  double temp;
  int *np;
  ASTNODE *ASTNODEp;

  sscanf(yytext, "%lf", &temp);

  ASTNODEp = yylval.np;

  if(strstr(yytext,".")){
    yylval.np = make_ast_double(temp);
    return DOUBLE_LITERAL;
  } else {
    yylval.np = make_ast_int((int )temp);
    return INT_LITERAL;
  }
  ASSERT_FAIL_BLOCK();
}
(int|double|bool|string) {
  char temp[7];
  sscanf(yytext, "%s", temp);

  yylval.type = get_type_by_name(temp);

  return TYPE_LITERAL;
}
(true|false) {
  char temp[7];
  BOOL bool;
  sscanf(yytext, "%s", temp);

  if(!strcmp(temp, "true")){
  bool = TRUE;
  } else {
  bool = FALSE;
  }

  yylval.np = make_ast_bool(bool);


  return BOOL_LITERAL;
}
[a-zA-Z]+[a-zA-Z0-9]* {
  char temp[VAR_NAME_MAX];

  sscanf(yytext, "%s", temp);

  yylval.str =  strdup(temp);

  return NAME;
}
%%
