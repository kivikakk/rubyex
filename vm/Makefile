CXXFLAGS := -I. -I../include -g -Wall
LDFLAGS := 

CPPS := $(wildcard *.cpp)
CXXS := $(wildcard *.cxx)
CPPOBJS := $(patsubst %.cpp,out/%.o,$(CPPS))
CXXOBJS := $(patsubst %.cxx,out/%.o,$(CXXS))

# .cpp files are linked into vm.a, which is shared.
# .cxx files are those specific for the `vm' executable.

vm: $(CXXOBJS) vm.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(CXXOBJS) vm.a

vm.a: $(CPPOBJS)
	ar rs $@ $(CPPOBJS)

out/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

out/%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	-rm out/*.o vm vm.a
