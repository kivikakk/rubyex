CXXFLAGS := -I. -I../include -g -Wall
LDFLAGS := 

CPPS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,out/%.o,$(CPPS))

program: program.a
	$(CXX) $(LDFLAGS) -o $@ program.a

program.a: $(OBJS)
	ar rs $@ $(OBJS)

out/%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	-rm out/*.o program
