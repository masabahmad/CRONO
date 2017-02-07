
TRGS = bfs bfs_atomic

LIBS += -lpthread -lrt

CXXFLAGS = -g --std=c++0x -O3
CXXFLAGS += -Wall

all: $(TRGS)

bfs: bfs.cc
bfs_atomic: bfs_atomic.cc

clean:
	rm -f $(TRGS) *.o *.dot
	rm -f myfile.txt

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
