
TRGS = triangle_counting

LIBS += -lpthread -lrt

CXXFLAGS = -g --std=c++0x -O2
CXXFLAGS += -Wall -Werror

all: $(TRGS)

triangle_counting: triangle_counting.cc

clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
