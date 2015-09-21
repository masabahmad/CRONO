
TRGS = triangle_counting_synthetic

LIBS += -lpthread -lrt

CXXFLAGS = -g --std=c++0x -O2

all: $(TRGS)

triangle_counting_synthetic: triangle_counting_synthetic.cc

clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
