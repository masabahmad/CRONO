
TRGS = community_lock community_atomic

LIBS += -lpthread -lrt

CXXFLAGS = -g --std=c++0x -O3
CXXFLAGS += -Wall -Werror

all: $(TRGS)

community_lock: community_lock.cc
community_atomic: community_atomic.cc

clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
