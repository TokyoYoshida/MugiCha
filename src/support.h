#ifndef SUPPORT_H
#define SUPPORT_H 1

#include <stdio.h>
#include <stdlib.h>


#define DEBUGL     printf("%s:%d\n",__FILE__,__LINE__)
#define DEBUGP(p)  printf("%s:%d addr = %p\n",__FILE__, __LINE__, p)
#define DEBUGS(m)  printf("%s:%d message = %s\n",__FILE__, __LINE__, m)
#define DEBUGI(i)  printf("%s:%d int data = %d\n",__FILE__, __LINE__, i)
#define DEBUGW(m)  printf("%s:%d label = %s",__FILE__, __LINE__, m)

#define ASSERT_FAIL(s)  fprintf(stderr, "%s:%d assertion fail (%s)\n",__FILE__, __LINE__, s), exit(1)
#define ASSERT_FAIL_BLOCK()  fprintf(stderr, "%s:%d assertion fail (this block expect never call.)\n",__FILE__, __LINE__), exit(1)
#define ASSERT_FAIL_MEMORY()  fprintf(stderr, "%s:%d assertion fail (memory error.)\n",__FILE__, __LINE__), exit(1)

#endif
