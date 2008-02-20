CXXFLAGS := -enable-rtti -I. -g -DYYDEBUG
LDFLAGS := 

CPPS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,out/%.o,$(CPPS))

parser: out/ruby.tab.o out/lex.yy.o $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ out/ruby.tab.o out/lex.yy.o $(OBJS)
	@./test 1> /dev/null

out/%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

out/ruby.tab.o: out/ruby.tab.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

out/lex.yy.o: out/lex.yy.c out/ruby.tab.c
	$(CXX) $(CXXFLAGS) -c -o $@ out/lex.yy.c

out/ruby.tab.c: ruby.y
	bison -v --report=state -d $<
	mv ruby.tab.? out/

out/lex.yy.c: ruby.l
	flex -t $< > $@

clean:
	-rm out/*.o out/*.tab.? out/lex.yy.c parser
