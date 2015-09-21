
TRGS = dfs_synthetic

LIBS += -lpthread -lrt

CXXFLAGS = -g --std=c++0x -O2

all: $(TRGS)

dfs_synthetic: dfs_synthetic.cc

clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
