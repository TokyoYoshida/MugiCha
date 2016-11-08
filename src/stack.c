#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "support.h"

static STACK *stackp;

void stack_init(STACK *sp)
{
  sp->pos = 0;
  stackp = sp;
}

void stack_push(STACK *sp, void *p)
{
  if(sp->pos == STACK_MAX)
    ASSERT_FAIL("assertion failed: stack overflow.");

  sp->ar[sp->pos++] = p;
}

void *stack_pop(STACK *sp)
{
  if(sp->pos == 0 )
    ASSERT_FAIL("assertion failed: stack can't pop beacuse of below zero.");

  return sp->ar[--sp->pos];
}

int stack_is_empty(STACK *sp)
{
  return sp->pos == 0;
}

void set_stack_pos(int newpos)
{
    stackp->pos = newpos;
}

int get_stack_pos()
{
    return stackp->pos;
}
