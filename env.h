#ifndef ENV_H
#define ENV_H 1

#define ENV_MAX 100

typedef struct _ENV {
  int stack_pos;
  int var_pos;
} ENV;

void push_env();
void pop_env();

#endif
