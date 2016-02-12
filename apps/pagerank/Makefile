
TRGS = pagerank

LIBS += -lpthread -lrt

CXXFLAGS = -g --std=c++0x -O2
CXXFLAGS += -Wall -Werror

all: $(TRGS)

pagerank: pagerank.cc

clean:
	rm -f $(TRGS) *.o *.dot
	rm -f file.txt

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
