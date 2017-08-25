%{
#include <stdio.h>
#include <stdlib.h>
#include "mugicha.h"
#include "support.h"
#include "llvm_builder.h"
#define YYDEBUG 1

static enum MugichaMode mugichaMode;

#ifdef YACC_DEBUG
int yydebug=1;
#endif

%}
%union {
    struct _ASTNODE    *np;
    char *str;
}
%token <np>      DOUBLE_LITERAL INT_LITERAL BOOL_LITERAL STRING_LITERAL
%token <str>     NAME
%token '+' '-' '*' '/' '\n' '(' ')' '.' '=' EQUAL PRINT VAR FUNCTION CLASSDEF NOTEQUAL '!' '<' '>' '\"'
%token SMALLEREQUAL GREATEREQUAL IF ELSE WHILE
%type <np> prog stmt expr expr_print def_var set_var set_member_var
%type <np> def_class def_vars def_funcs
%type <np> def_func call_func def_method call_method primary_bool expr_cmp_eq expr_cmp_noteq
%type <np> expr_cmp_greater expr_cmp_smaller expr_cmp_greaterequal expr_cmp_smallerequal
%type <np> if_stmt expr_bool while_stmt primary_double expr_double primary_string
%type <np> expr_string expr_int primary_int primary_get_variable primary_get_member_var
%right '='
%right '!'
%left '+' '-'
%left '*' '/'
%left '.'
%left EQUAL NOTEQUAL '<' '>' SMALLEREQUAL GREATEREQUAL

%start prog

%%
prog
    : /* empty */
    | stmt
    {
      if( mugichaMode == Compiler){
        mugicha_compile($1);
      } else if( mugichaMode == DisplayAst){
        display_ast($1);
      }
    }
    ;
stmt
    : expr
    | def_var
    | set_var
    | set_member_var
    | def_func
    | def_method
    | def_class
    | if_stmt
    | while_stmt
    | stmt stmt
    {
      $$ = make_ast_op(SEQ, $1, $2);
    }
    ;
def_class
    : CLASSDEF NAME '{' def_vars def_funcs '}'
    {
    $$ = make_ast_def_class($2, $4, $5);
    }
    | CLASSDEF NAME '{' def_vars '}'
    {
    $$ = make_ast_def_class($2, $4, NULL);
    }
    | CLASSDEF NAME '{' def_funcs '}'
    {
    $$ = make_ast_def_class($2, NULL, $4);
    }
    | CLASSDEF NAME '{' '}'
    {
    $$ = make_ast_def_class($2, NULL, NULL);
    }
    ;
def_vars
    : def_var
    | def_vars def_var
    {
    $$ = make_ast_op(SEQ, $1, $2);
    }
    ;
def_funcs
    : def_func
    | def_funcs def_func
    {
    $$ = make_ast_op(SEQ, $1, $2);
    }
    ;
def_func
    : FUNCTION NAME '(' def_var ')' NAME '{' stmt '}'
    {
    $$ = make_ast_def_func($2, $4, $6, $8);
    }
    | FUNCTION NAME '(' ')' NAME '{' stmt '}'
    {
    $$ = make_ast_def_func($2, NULL, $5, $7);
    }
    ;
def_method
    : FUNCTION NAME '.' NAME '(' def_var ')' NAME '{' stmt '}'
    {
    TMP_DEBUGL;
    $$ = make_ast_def_method($2, $4, $6, $8, $10);
    }
    | FUNCTION NAME '.' NAME '(' ')' NAME '{' stmt '}'
    {
    TMP_DEBUGL;
    $$ = make_ast_def_method($2, $4, NULL, $7, $9);
    }
    ;
def_var
    : VAR NAME NAME
    {
    $$ = make_ast_def_var($2, $3);
    }
    ;
set_var
    : NAME '=' expr
    {
    $$ = make_ast_set_var($1, $3);
    }
    ;
set_member_var
    : NAME '.' NAME '=' expr
    {
    $$ = make_ast_set_member_var($1, $3, $5);
    }
    ;
if_stmt
    : IF expr_bool '{' stmt '}' ELSE '{' stmt '}'
    {
    $$ = make_ast_if($2, $4 ,$8);
    }
    ;
while_stmt
    : WHILE expr_bool '{' stmt '}'
    {
    $$ = make_ast_while($2, $4 );
    }
    ;
expr
    : expr_int
    | expr_print
    | expr_bool
    | expr_double
    | expr_string
    | call_func
    | call_method
    | primary_get_variable
    | primary_get_member_var
    | expr '+' expr
    {
      $$ = make_ast_op(ADD, $1, $3);
    }
    | expr '-' expr
    {
      $$ = make_ast_op(SUB, $1, $3);
    }
    | expr '*' expr
    {
      $$ = make_ast_op(MUL, $1, $3);
    }
    | expr '/' expr
    {
      $$ = make_ast_op(DIV, $1, $3);
    }
    ;
call_func
    : NAME '(' set_var ')'
    {
    $$ = make_ast_call_func($1, $3);
    }
    | NAME '(' ')'
    {
    $$ = make_ast_call_func($1, NULL);
    }
    ;
call_method
    : NAME '.' NAME '(' set_var ')'
    {
    $$ = make_ast_call_method($1, $3, $5);
    }
    | NAME '(' ')'
    {
    $$ = make_ast_call_method($1, $3, NULL);
    }
    ;
expr_print
    : PRINT '(' stmt ')'
    {
      $$ = make_ast_cmd(PRINTDATA, $3);
    }
    | PRINT '(' expr ')'
    {
      $$ = make_ast_cmd(PRINTDATA, $3);
    }
    ;
expr_int
    : expr_int '+' expr_int
    {
      $$ = make_ast_op(ADD, $1, $3);
    }
    | expr_int '-' expr_int
    {
      $$ = make_ast_op(SUB, $1, $3);
    }
    | expr_int '*' expr_int
    {
      $$ = make_ast_op(MUL, $1, $3);
    }
    | expr_int '/' expr_int
    {
      $$ = make_ast_op(DIV, $1, $3);
    }
    | primary_int
    ;
expr_double
    : expr_double '+' expr_double
    {
      $$ = make_ast_op(ADD, $1, $3);
    }
    | expr_double '-' expr_double
    {
      $$ = make_ast_op(SUB, $1, $3);
    }
    | expr_double '*' expr_double
    {
      $$ = make_ast_op(MUL, $1, $3);
    }
    | expr_double '/' expr_double
    {
      $$ = make_ast_op(DIV, $1, $3);
    }
    | primary_double
    ;
expr_string
    : expr_string '+' expr_string
    {
      $$ = make_ast_op(ADD, $1, $3);
    }
    | expr_string '+' expr_int
    {
      $$ = make_ast_op(ADD, $1, $3);
    }
    | expr_string '+' expr_double
    {
      $$ = make_ast_op(ADD, $1, $3);
    }
    | expr_string '+' expr_bool
    {
      $$ = make_ast_op(ADD, $1, $3);
    }
    | primary_string
    ;
expr_bool
    : primary_bool
    | expr_cmp_eq
    | expr_cmp_noteq
    | expr_cmp_greater
    | expr_cmp_smaller
    | expr_cmp_greaterequal
    | expr_cmp_smallerequal
    ;
expr_cmp_eq
    : expr EQUAL expr
    {
    $$ = make_ast_op(CMP_EQ, $1 ,$3);
    }
    ;
expr_cmp_noteq
    : expr NOTEQUAL expr
    {
    $$ = make_ast_op(CMP_NOTEQ, $1 ,$3);
    }
    ;
expr_cmp_greater
    : expr '>' expr
    {
    $$ = make_ast_op(CMP_GREATER, $1 ,$3);
    }
    ;
expr_cmp_smaller
    : expr '<' expr
    {
    $$ = make_ast_op(CMP_SMALLER, $1 ,$3);
    }
    ;
expr_cmp_greaterequal
    : primary_int GREATEREQUAL primary_int
    {
    $$ = make_ast_op(CMP_GREATEREQ, $1 ,$3);
    }
    ;
expr_cmp_smallerequal
    : primary_int SMALLEREQUAL primary_int
    {
    $$ = make_ast_op(CMP_SMALLEREQ, $1 ,$3);
    }
    ;
primary_int
    : INT_LITERAL
    ;
primary_double
    : DOUBLE_LITERAL
    ;
primary_string
    : STRING_LITERAL
    ;
primary_bool
    : BOOL_LITERAL
    ;
primary_get_variable
    : NAME
    {
      $$ = make_ast_get_var($1);
    }
    ;
primary_get_member_var
    : NAME '.' NAME
    {
      $$ = make_ast_get_member_var($1, $3);
    }
    ;
%%

int
yyerror(char const *str)
{
    extern char *yytext;
    fprintf(stderr, "parser error near %s\n", yytext);
    return 0;
}

int main(int argc,char *argv[])
{
    extern int yyparse(void);
    extern FILE *yyin;

    if(argc > 1){
      if(!strcmp(argv[1],"i")){
        mugichaMode = Interpreter;
      } else if(!strcmp(argv[1],"c")){
        mugichaMode = Compiler;
      } else if(!strcmp(argv[1],"a")){
        mugichaMode = DisplayAst;
      }
    }

    yyin = stdin;
    if (yyparse()) {
        fprintf(stderr, "Symtax Error\n");
        exit(1);
    }

    return 0;
}
