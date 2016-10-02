#ifndef EVAL_H
#define EVAL_H 1

#include "ast.h"

VALUE exec_add(ASTNODE *dp);
VALUE eval_node_value(ASTNODE *dp);
VALUE eval_node_op(ASTNODE *dp);
VALUE eval_node(ASTNODE *ap);

#endif
