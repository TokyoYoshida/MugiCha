#define STACK_MAX 100

typedef struct _stack {
  void *ar[STACK_MAX];
  int pos;
} STACK;

void stack_init(STACK *sp);
void stack_push(STACK *sp, void *p);
void *stack_pop(STACK *sp);
int stack_is_empty(STACK *sp);

void set_stack_pos(int newpos);
int get_stack_pos();
