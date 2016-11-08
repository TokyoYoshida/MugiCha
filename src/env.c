#include "env.h"
#include "stack.h"
#include "var.h"

static ENV envs[ENV_MAX];
static int pos;

void save_env()
{
//  envs[pos].stack_pos = get_stack_pos();
  envs[pos].var_pos = get_var_pos();
}

void restore_env()
{
//  set_stack_pos( envs[pos].stack_pos);
  set_var_pos( envs[pos].var_pos);
}

void push_env()
{
  save_env();
  pos++;
}

void pop_env()
{
  pos--;
  restore_env();
}
