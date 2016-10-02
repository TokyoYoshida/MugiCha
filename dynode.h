#ifndef DYNODE_H
#define DYNODE_H 1

#include "ast.h"

typedef struct _DYNODE {
    TYPE          type;
    OPERATION     op;
    void         *val;
    BOOL         evaled;
    SYMBOL       *sym;
    struct _ASTNODE *ast;
    struct _DYNODE  *left;
    struct _DYNODE  *right;
    struct _DYNODE  *def_args;
    struct _DYNODE  *set_args;
} DYNODE;

void print_dynode(int depth, DYNODE *np);
DYNODE *make_dynode(ASTNODE *ast);
void cleanup_dynode(DYNODE *p);
void make_dynode_sub(DYNODE *dp);
void print_dynodetree(int depth, DYNODE *np);

#endif
