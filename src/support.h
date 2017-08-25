#ifndef SUPPORT_H
#define SUPPORT_H 1

#include <stdio.h>
#include <stdlib.h>

// #define MUGICHA_DEBUG 1

#ifdef MUGICHA_DEBUG

#define DEBUGL     fprintf(stderr, "%s:%d\n",__FILE__,__LINE__)
#define DEBUGP(p)  fprintf(stderr, "%s:%d addr = %p\n",__FILE__, __LINE__, p)
#define DEBUGS(m)  fprintf(stderr, "%s:%d message = %s\n",__FILE__, __LINE__, m)
#define DEBUGI(i)  fprintf(stderr, "%s:%d int data = %d\n",__FILE__, __LINE__, i)
#define DEBUGW(m)  fprintf(stderr, "%s:%d label = %s",__FILE__, __LINE__, m)

// Use for temporary work only
#define TMP_DEBUGL     fprintf(stderr, "%s:%d\n",__FILE__,__LINE__)
#define TMP_DEBUGP(p)  fprintf(stderr, "%s:%d addr = %p\n",__FILE__, __LINE__, p)
#define TMP_DEBUGS(m)  fprintf(stderr, "%s:%d message = %s\n",__FILE__, __LINE__, m)
#define TMP_DEBUGI(i)  fprintf(stderr, "%s:%d int data = %d\n",__FILE__, __LINE__, i)
#define TMP_DEBUGW(m)  fprintf(stderr, "%s:%d label = %s",__FILE__, __LINE__, m)

#else

#define DEBUGL  ((void)0)
#define DEBUGP(p)  ((void)0)
#define DEBUGS(m)  ((void)0)
#define DEBUGI(i)  ((void)0)
#define DEBUGW(m)  ((void)0)

#define TMP_DEBUGL  ((void)0)
#define TMP_DEBUGP(p)  ((void)0)
#define TMP_DEBUGS(m)  ((void)0)
#define TMP_DEBUGI(i)  ((void)0)
#define TMP_DEBUGW(m)  ((void)0)

#endif

#define ASSERT_FAIL(s)  fprintf(stderr, "%s:%d assertion fail (%s)\n",__FILE__, __LINE__, s), exit(1)
#define ASSERT_FAIL_BLOCK()  fprintf(stderr, "%s:%d assertion fail (this block expect never call.)\n",__FILE__, __LINE__), exit(1)
#define ASSERT_FAIL_MEMORY()  fprintf(stderr, "%s:%d assertion fail (memory error.)\n",__FILE__, __LINE__), exit(1)

#endif
