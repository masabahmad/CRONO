
TRGS = connected_synthetic connected_real

LIBS += -lpthread -lrt

CXXFLAGS = -g --std=c++0x -O2

all: $(TRGS)

connected_synthetic: connected_synthetic.cc

connected_real: connected_real.cc

clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
