CXXFLAGS := -I. -I../include -g -Wall
LDFLAGS := 

CPPS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,out/%.o,$(CPPS))

program: program.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ main.cxx program.a

program.a: $(OBJS)
	ar rs $@ $(OBJS)

out/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	-rm out/*.o program program.a
