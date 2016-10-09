
TRGS = connected_components_lock connected_components_atomic

LIBS += -lpthread -lrt

CXXFLAGS = -g --std=c++0x -O3
CXXFLAGS += -Wall -Werror

all: $(TRGS)

connected_components_lock: connected_components_lock.cc
connected_components_atomic: connected_components_atomic.cc

clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
