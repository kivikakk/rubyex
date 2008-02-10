CXXFLAGS := -enable-rtti -I.
LDFLAGS := 

ruby: out/ruby.tab.o out/lex.yy.o out/expr.o out/pretty.o
	$(CXX) $(LDFLAGS) -o $@ out/ruby.tab.o out/lex.yy.o out/expr.o out/pretty.o

out/expr.o: expr.cpp expr.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

out/pretty.o: pretty.cpp pretty.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

out/ruby.tab.o: out/ruby.tab.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

out/lex.yy.o: out/lex.yy.c out/ruby.tab.c
	$(CXX) $(CXXFLAGS) -c -o $@ out/lex.yy.c

out/ruby.tab.c: ruby.y
	bison --report=state -d $<
	mv ruby.tab.? out/

out/lex.yy.c: ruby.lex
	flex -t $< > $@

clean:
	-rm out/*.o out/*.tab.? out/lex.yy.c ruby
