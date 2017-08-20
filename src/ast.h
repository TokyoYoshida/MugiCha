#ifndef NODE_H
#define NODE_H 1

#include "type.h"
#include "symbol.h"

#define VAR_NAME_MAX 20

typedef struct _ASTNODE {
    TYPE          type;
    OPERATION     op;
    VALUE         val;
    SYMBOL        *sym;
    SYMBOL        *member;
    struct _ASTNODE *left;
    struct _ASTNODE *right;
    struct _ASTNODE *set_args;
    struct _ASTNODE *condition;
    struct _ASTNODE *def_vars;
    struct _ASTNODE *def_funcs;
} ASTNODE;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
ASTNODE *make_ast_int(int val);
ASTNODE *make_ast_bool(BOOL val);
ASTNODE *make_ast_double(double val);
ASTNODE *make_ast_string(char *val);

ASTNODE *make_ast_op(OPERATION op, ASTNODE *lhr, ASTNODE *rhr);
ASTNODE *make_ast_cmd(OPERATION op, ASTNODE *argp);
ASTNODE *make_astnode();

ASTNODE *make_ast_op(OPERATION op, ASTNODE *lhr, ASTNODE *rhr);
ASTNODE *make_ast_cmd(OPERATION op, ASTNODE *argp);
ASTNODE *make_ast_def_var(char *name, char *type_name);
ASTNODE *make_ast_set_var(char *name, ASTNODE *rhr);
ASTNODE *make_ast_set_member_var(char *var_name, char *member_name, ASTNODE *newval);
ASTNODE *make_ast_get_member_var(char *var_name, char *member_name);
ASTNODE *make_ast_get_var(char *name);
ASTNODE *make_ast_def_func(char *name, ASTNODE *def_args, char *type_name, ASTNODE *body);
ASTNODE *make_ast_def_class(char *name, ASTNODE *def_vars,ASTNODE *def_funcs);

ASTNODE *make_ast_call_func(char *name, ASTNODE *set_args);
ASTNODE *make_ast_if(ASTNODE *cond, ASTNODE *then_stmt, ASTNODE *else_stmt);
ASTNODE *make_ast_while(ASTNODE *cond, ASTNODE *loop_stmt);

void print_astnode(int depth, ASTNODE *np);
void print_astnodeln(int depth, ASTNODE *np);
void print_ast(int depth, ASTNODE *np);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
