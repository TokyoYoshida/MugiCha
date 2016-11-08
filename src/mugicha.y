%{
#include <stdio.h>
#include <stdlib.h>
#include "mugicha.h"
#include "support.h"
#include "llvm_builder.h"
#define YYDEBUG 1

static enum MugichaMode mugichaMode;

%}
%union {
    struct _ASTNODE    *np;
    char *str;
    int type;
}
%token <np>      DOUBLE_LITERAL INT_LITERAL BOOL_LITERAL
%token <str>     NAME STRING_LITERAL
%token <type>    TYPE_LITERAL
%token '+' '-' '*' '/' '\n' '(' ')' '=' EQUAL PRINT VAR FUNCTION NOTEQUAL '!' '<' '>' '\"'
%token SMALLEREQUAL GREATEREQUAL IF ELSE WHILE
%type <np> prog stmt expr expr_int primary_int expr_print def_var set_var
%type <np> def_func call_func primary_bool expr_cmp_eq expr_cmp_noteq
%type <np> expr_cmp_greater expr_cmp_smaller expr_cmp_greaterequal expr_cmp_smallerequal
%type <np> if_stmt expr_bool while_stmt primary_double expr_double primary_string
%type <np> expr_string
%left '+' '-'
%left '*' '/'
%left EQUAL NOTEQUAL '!' '<' '>' SMALLEREQUAL GREATEREQUAL
%right '='

%start prog

%%
prog
    : /* empty */
    | stmt
    {
      if( mugichaMode == Interpreter){
        mugicha_main($1);
      } else {
        mugicha_compile($1);
      }
    }
    ;
stmt
    : expr
    | def_var
    | set_var
    | def_func
    | if_stmt
    | while_stmt
    | stmt stmt
    {
      $$ = make_ast_op(SEQ, $1, $2);
    }
    ;
def_func
    : FUNCTION NAME '(' def_var ')' TYPE_LITERAL '{' stmt '}'
    {
    $$ = make_ast_def_func($2, $4, $6, $8);
    }
    | FUNCTION NAME '(' ')' TYPE_LITERAL '{' stmt '}'
    {
    $$ = make_ast_def_func($2, NULL, $5, $7);
    }
    ;
def_var
    : VAR NAME TYPE_LITERAL
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
    : primary_int EQUAL primary_int
    {
    $$ = make_ast_op(CMP_EQ, $1 ,$3);
    }
    | primary_bool EQUAL primary_bool
    {
    $$ = make_ast_op(CMP_EQ, $1 ,$3);
    }
    ;
expr_cmp_noteq
    : primary_int NOTEQUAL primary_int
    {
    $$ = make_ast_op(CMP_NOTEQ, $1 ,$3);
    }
    | primary_bool NOTEQUAL primary_bool
    {
    $$ = make_ast_op(CMP_NOTEQ, $1 ,$3);
    }
    ;
expr_cmp_greater
    : primary_int '>' primary_int
    {
    $$ = make_ast_op(CMP_GREATER, $1 ,$3);
    }
    | primary_bool '>' primary_bool
    {
    $$ = make_ast_op(CMP_GREATER, $1 ,$3);
    }
    ;
expr_cmp_smaller
    : primary_int '<' primary_int
    {
    $$ = make_ast_op(CMP_SMALLER, $1 ,$3);
    }
    | primary_bool '<' primary_bool
    {
    $$ = make_ast_op(CMP_SMALLER, $1 ,$3);
    }
    ;
expr_cmp_greaterequal
    : primary_int GREATEREQUAL primary_int
    {
    $$ = make_ast_op(CMP_GREATEREQ, $1 ,$3);
    }
    | primary_bool GREATEREQUAL primary_bool
    {
    $$ = make_ast_op(CMP_GREATEREQ, $1 ,$3);
    }
    ;
expr_cmp_smallerequal
    : primary_int SMALLEREQUAL primary_int
    {
    $$ = make_ast_op(CMP_SMALLEREQ, $1 ,$3);
    }
    | primary_bool SMALLEREQUAL primary_bool
    {
    $$ = make_ast_op(CMP_SMALLER, $1 ,$3);
    }
    ;
primary_int
    : INT_LITERAL
    | NAME
    {
      $$ = make_ast_get_var($1);
    }
    ;
primary_double
    : DOUBLE_LITERAL
    | NAME
    {
      $$ = make_ast_get_var($1);
    }
    ;
primary_string
    : STRING_LITERAL
    | NAME
    {
      $$ = make_ast_get_var($1);
    }
    ;
primary_bool
    : BOOL_LITERAL
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
      }
    }

    yyin = stdin;
    if (yyparse()) {
        fprintf(stderr, "Symtax Error\n");
        exit(1);
    }

    return 0;
}
