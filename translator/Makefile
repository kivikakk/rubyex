
ruby: out/ruby.tab.o out/lex.yy.o out/expr.o
	$(CXX) -lm -o $@ out/ruby.tab.o out/lex.yy.o out/expr.o

out/expr.o: expr.cpp
	$(CXX) -c -o $@ $<

out/ruby.tab.o: out/ruby.tab.c
	$(CXX) -c -I. -o $@ $<

out/lex.yy.o: out/lex.yy.c out/ruby.tab.c
	$(CXX) -c -I. -o $@ out/lex.yy.c

out/ruby.tab.c: ruby.y
	bison --report=state -d $<
	mv ruby.tab.? out/

out/lex.yy.c: ruby.lex
	flex -t $< > $@

clean:
	-rm out/*.o out/*.tab.? out/lex.yy.c ruby
