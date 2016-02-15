
TRGS = tsp

LIBS += -lpthread -lrt

CXXFLAGS = -g --std=c++0x -O2
CXXFLAGS += -Wall -Werror

all: $(TRGS)

tsp: tsp.cc

clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
