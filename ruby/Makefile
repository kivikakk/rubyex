CXXFLAGS := -I. -I../include -g -Wall -I.. -DNO_MEMBER_TEMPLATES
LDFLAGS := 

CPPS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,out/%.o,$(CPPS))

ruby: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) ../vm/vm.a ../parser/parser.a

out/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	-rm out/*.o ruby

relink:
	-rm ruby
	@make ruby
