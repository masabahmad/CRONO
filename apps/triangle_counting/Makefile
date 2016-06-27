
TRGS = triangle_counting_lock triangle_counting_atomic

LIBS += -lpthread -lrt

CXXFLAGS = -g --std=c++0x -O2
CXXFLAGS += #-Wall -Werror

all: $(TRGS)

triangle_counting_lock: triangle_counting_lock.cc
triangle_counting_atomic: triangle_counting_atomic.cc

clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
