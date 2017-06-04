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
MUGICHAC = script/mugichac
TESTS = $(wildcard test/*.mugi)
RESULTS = $(TESTS:%.mugi=%.result)
EXPECTS = $(TESTS:%.mugi=%.expect)
DIFFS = $(TESTS:%.mugi=%.diff)

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
	$(YACC) $< -o $(YACCC) -d -t

$(LEXC): $(LEXS)
	$(LEX) -t $< > $(LEXC)

%.o: %.c $(YACCC) $(LEXC)
	$(CC) -c -MMD -MP -o $@ $<

%.o: %.cpp
	$(CPP) -c $(CPPFLG) -o $@ $<

clean:
	rm -f $(PROG) $(OBJS) $(DEPS) $(YACCO) $(LEXO) $(YACCC) $(LEXC) $(YACCH) $(RESULTS) $(DIFFS)

clean_test:
	rm -f $(EXPECTS)

disp_ast:
	./mugicha_exec a < sample.mugi

run_interpreter:
	./mugicha_exec i < sample.mugi

run_compiler:
	$(MUGICHAC) sample.mugi

test: $(DIFFS)
	echo "The test is successful."

%.diff: %.result %.expect
	diff $(basename $<).result $(basename $<).expect > $@

$(RESULTS): $(TESTS)
	$(MUGICHAC) $< > $@

$(EXPECTS): $(TESTS)
	$(MUGICHAC) $< > $@
