
TRGS = pagerank

LIBS += -lpthread -lrt

CXXFLAGS = -g --std=c++0x -O2

all: $(TRGS)

pagerank: pagerank.cc

clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
