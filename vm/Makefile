CXXFLAGS := -I. -I../include -g -Wall
LDFLAGS := 

CPPS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,out/%.o,$(CPPS))

vm: vm.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ main.cxx vm.a

vm.a: $(OBJS)
	ar rs $@ $(OBJS)

out/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	-rm out/*.o vm vm.a
