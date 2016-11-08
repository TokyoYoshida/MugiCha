.SUFFIXES:

PROG = mugicha_exec
LEXS  = $(wildcard src/*.l)
YACCS = $(wildcard src/*.y)
LEXC  = src/lex.yy.c
YACCC = src/y.tab.c
YACCH = src/y.tab.h
LEXO  = src/lex.yy.o
YACCO = src/y.tab.o
CSRCS = $(wildcard src/*.c)
CPPSRCS = $(wildcard src/*.cpp)
OBJS = $(CSRCS:%.c=%.o) $(CPPSRCS:%.cpp=%.o) $(LEXO) $(YACCO)
DEPS = $(CSRCS:%.c=%.d) $(CPPSRCS:%.cpp=%.d)

CC   = clang
CPP  = clang++
CPPFLG = -g -O3 -std=c++11 -fno-exceptions -fno-rtti $1 `llvm-config --cppflags`
LDFLG = -ll -ly -g -O3 -std=c++11 -fno-exceptions -fno-rtti $1 `llvm-config --ldflags --system-libs --libs core executionengine interpreter mc support nativecodegen`
LEX  = lex
YACC = yacc

all: $(PROG)

-include $(DEPS)

$(PROG): $(OBJS)
	$(CPP) $(LDFLG) -o $(PROG) $^

$(YACCC): $(YACCS)
	$(YACC) $< -o $(YACCC) -d

$(LEXC): $(LEXS)
	$(LEX) -t $< > $(LEXC)

%.o: %.c $(YACCC) $(LEXC)
	$(CC) -c -MMD -MP -o $@ $<

%.o: %.cpp
	$(CPP) -c $(CPPFLG) -o $@ $<

clean:
	rm -f $(PROG) $(OBJS) $(DEPS) $(YACCO) $(LEXO) $(YACCC) $(LEXC) $(YACCH)

run_interpreter:
	./mugicha_exec i < sample.mugi

run_compiler:
	./mugicha_exec c < sample.mugi > sample.bc && lli sample.bc
