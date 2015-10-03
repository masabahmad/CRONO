
TRGS = triangle_counting

LIBS += -lpthread -lrt

CXXFLAGS = -g -w --std=c++0x -O2

all: $(TRGS)

triangle_counting: triangle_counting.cc

clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
