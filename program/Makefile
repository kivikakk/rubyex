CXXFLAGS := -I. -I../include -g -Wall
LDFLAGS := 

CPPS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,out/%.o,$(CPPS))

program: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

out/%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	-rm out/*.o program
