
TRGS = range_dijk_real range_dijk boost_dijk_real

LIBS += -lpthread -lrt

##BOOST_PATH = /run/pkg/boost-/1.53.0-gcc_4.7.3

##BOOST_FLAGS = -I$(BOOST_PATH)/include -L$(BOOST_PATH)/lib -lboost_graph

CXXFLAGS = -g --std=c++11

all: $(TRGS)

range_dijk_real: range_dijk_real.cc

range_dijk: range_dijk.cc

boost_dijk_real: boost_dijk_real.cc

clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)
