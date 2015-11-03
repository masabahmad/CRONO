
TRGS = community

LIBS += -lpthread -lrt

CXXFLAGS = -g --std=c++0x -O2
CXXFLAGS += -Wall -Werror

all: $(TRGS)

community: community.cc

clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
