
TRGS = connected_components

LIBS += -lpthread -lrt

CXXFLAGS = -g --std=c++0x -O2
CXXFLAGS += -Wall -Werror

all: $(TRGS)

connected_components: connected_components.cc

clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
