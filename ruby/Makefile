CXXFLAGS := -I. -I../include -g -Wall
LDFLAGS := ../program/program.a ../translator/parser.a

CPPS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,out/%.o,$(CPPS))

ruby: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

out/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	-rm out/*.o ruby
