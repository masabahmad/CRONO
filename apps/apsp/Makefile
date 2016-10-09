
TRGS = apsp

LIBS += -lpthread -lrt

##BOOST_PATH = /run/pkg/boost-/1.53.0-gcc_4.7.3

##BOOST_FLAGS = -I$(BOOST_PATH)/include -L$(BOOST_PATH)/lib -lboost_graph

CXXFLAGS = -g --std=c++0x -O3
CXXFLAGS += -Wall -Werror

all: $(TRGS)

apsp: apsp.cc

##boost_dijk_real: boost_dijk_real.cc

clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
