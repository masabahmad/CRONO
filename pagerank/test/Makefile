
TRGS = boost-pr

LIBS += -lpthread -lrt

BOOST_PATH = /run/pkg/boost-/1.53.0-gcc_4.7.3

BOOST_FLAGS = -I$(BOOST_PATH)/include -L$(BOOST_PATH)/lib -Wl,-R$(BOOST_PATH)/lib 

BOOST_LIBS = -lboost_program_options

CXXFLAGS = -g --std=c++0x $(BOOST_FLAGS)

all: $(TRGS)

boost-pr: boost-pr.cc

clean:
	rm -f $(TRGS) *.o *.dot

%: %.cc ;
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS) $(BOOST_LIBS)
