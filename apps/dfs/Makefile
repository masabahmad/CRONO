
TRGS = dfs

LIBS += -lpthread -lrt

CXXFLAGS = -g --std=c++0x -O3
CXXFLAGS += -Wall -Werror

all: $(TRGS)

dfs: dfs.cc

clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
