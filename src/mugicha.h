#ifndef mugicha_H
#define mugicha_H 1

#include "ast.h"
#include "dynode.h"
#include "eval.h"

enum MugichaMode {Interpreter, Compiler, DisplayAst};

// #define DEBUG 1

void interpreter_main(ASTNODE *rootp);
void mugicha_main(ASTNODE *rootp);
void cleanup_astnodes(ASTNODE *np);
void display_ast(ASTNODE *rootp);

#endif
