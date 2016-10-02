
static FUNC *call_stack[CALL_STACK_MAX];
static int pos;

void push_call_stack(FUNC *f)
{
  call_stack[pos++] = f;
}

FUNC *pop_call_stack()
{
  return call_stack[pos--];
}
