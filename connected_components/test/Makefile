
TRGS = connected_test

LIBS += -lpthread -lrt

CXXFLAGS = -g --std=c++0x -O2

all: $(TRGS)

connected_test: connected_test.cc
clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
