
ruby: out/ruby.tab.o out/lex.yy.o
	gcc -lm -o $@ out/ruby.tab.o out/lex.yy.o

out/ruby.tab.o: out/ruby.tab.c
	gcc -c -o $@ $<

out/lex.yy.o: out/lex.yy.c out/ruby.tab.c
	gcc -c -o $@ out/lex.yy.c

out/ruby.tab.c: ruby.y
	bison -d $<
	mv ruby.tab.? out/

out/lex.yy.c: ruby.lex
	flex -t $< > $@

clean:
	-rm out/*.o out/*.tab.? out/lex.yy.c ruby
