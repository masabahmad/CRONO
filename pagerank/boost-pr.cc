//
// This is a C++ program for running PageRank on a graph read from a
// SNAP-formatted file.  This uses the Boost Graph Library for the actual
// algorithm.  To build, change to 'native' and type make.
//


#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <exception>
#include <unordered_map>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/page_rank.hpp>
#include <boost/property_map/vector_property_map.hpp>

#include <boost/program_options.hpp>



#define PR_LABELS

using namespace std;
using namespace boost;
namespace po = boost::program_options;

typedef adjacency_list < vecS, vecS, directedS, no_property, property < edge_weight_t, int > > graph_t;
typedef property_map<graph_t, edge_weight_t>::type weight_map_t;
typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
typedef graph_traits < graph_t >::edge_descriptor edge_descriptor;
typedef std::pair<int, int> Edge;
typedef std::unordered_map<unsigned,unsigned> VertexHash;
typedef std::vector<unsigned> VertexMap;

#define BILLION 1E9

void create_dot_file(const string &fn,graph_t &g,const vector_property_map<double> &pageranks,const VertexMap &vertex_set)
{
  std::ofstream dot_file(fn.c_str());

  if (!dot_file) throw runtime_error("Could not open dot file " + fn);

  property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);

  dot_file << "digraph D {\n"
    << "  rankdir=LR\n"
    << "  size=\"4,3\"\n"
    << "  ratio=\"fill\"\n"
    << "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\",style=\"filled\"]\n";

  graph_traits < graph_t >::edge_iterator ei, ei_end;
  for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
    graph_traits < graph_t >::edge_descriptor e = *ei;
    graph_traits < graph_t >::vertex_descriptor
      u = source(e, g), v = target(e, g);
    dot_file << vertex_set[u] << " -> " << vertex_set[v] << '\n';
  }

# ifdef PR_LABELS
  graph_traits < graph_t >::vertex_iterator vi, vend;
  for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
    dot_file << vertex_set[*vi] << "[label=<" << vertex_set[*vi] << "  <font point-size=\"12\"><font color=\"blue\">" << pageranks[*vi] << "</font></font>>]\n";
  }
# endif
 
  dot_file << "}\n";
}

unsigned update_vertex_mapping(unsigned n,VertexHash &vhash,VertexMap &vertex_set,unsigned &vcount)
{
  auto iter = vhash.find(n);
  if (iter == vhash.end()) {
    unsigned sn = vcount++;
    vhash.insert(make_pair(n,sn));
    vertex_set.push_back(n);
    return sn;
  } else {
    return iter->second;
  }
}


void read_graph(const string &fn,graph_t &g,VertexMap &vertex_set)
{
  ifstream in(fn);
  if (!in) throw runtime_error("Could not open graph file " + fn);

  VertexHash vhash;
  vertex_set.clear();
  unsigned vcount = 0;

  string line;
  while (!in.eof()) {
    getline(in,line);

    if (line.empty() || line[0] == '#') continue;

    istringstream is(line);
    unsigned n0,n1;

    is >> n0 >> n1;

    n0 = update_vertex_mapping(n0,vhash,vertex_set,vcount);
    n1 = update_vertex_mapping(n1,vhash,vertex_set,vcount);

    add_edge(n0,n1,g);
  }
}

void write_pageranks(graph_t &g,const vector_property_map<double> &pr,const VertexMap &vertex_set)
{
  vector<pair<unsigned,unsigned> > vmapping;
  unsigned i = 0;
  for ( auto &iter : vertex_set ) {
    vmapping.push_back(make_pair(i,vertex_set[i]));
    ++i;
  }
  sort(vmapping.begin(),vmapping.end(),
       [](const pair<unsigned,unsigned> &x,const pair<unsigned,unsigned> &y) { return x.second < y.second; });

	ofstream myfile;
	myfile.open ("pr_boost.txt");
  cout << "pageranks:" << std::endl;
  for ( auto &iter : vmapping ) {
    //cout << "pr(" << iter.second << ") = " << pr[iter.first] << "\n";
    myfile << "pr(" << iter.second << ") = " << pr[iter.first] << "\n";
	}
  cout << '\n';
}

int main(int argc, char *argv[])
{
  try {
    bool quiet;
    string input_filename, graph_filename;
    unsigned iterations;
    double damping;

    po::options_description desc(string(argv[0]) + " usage: [options] <input-file>");
    desc.add_options()
      ("help,h",                                                               "Display short help.")
      ("quiet,q",        po::value<bool>(&quiet)->default_value(false),        "Quiet- do not print distances to standard out.")
      ("iterations,i",   po::value<unsigned>(&iterations)->default_value(16),  "PageRank iteration count.")
      ("damping,d",      po::value<double>(&damping)->default_value(0.85),     "PageRank damping factor.")
      ("graph-file",     po::value<string>(&graph_filename),                   "Write a dotty graph file.")
      ("input",          po::value<string>(&input_filename),                   "The input graph file.");

    po::positional_options_description p;
    p.add("input",-1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc,argv).options(desc).positional(p).run(),vm);
    po::notify(vm);

    if (argc < 2 || vm.count("help")) {
      cout << desc << '\n';
      return 1;
    }

    if (input_filename.empty()) {
      throw runtime_error("No input file specfied.");
    }

    graph_t g;
    VertexMap vertex_set;

    read_graph(input_filename,g,vertex_set);

    if (num_vertices(g) == 0) {
      throw runtime_error("Graph is empty!");
    }

    vector_property_map<double> pr(num_vertices(g));

    struct timespec requestStart, requestEnd;
    clock_gettime(CLOCK_REALTIME, &requestStart);

    graph::n_iterations done(iterations);

    page_rank(g,pr,done,damping);

    clock_gettime(CLOCK_REALTIME, &requestEnd);
	  double accum = ( requestEnd.tv_sec - requestStart.tv_sec ) + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;
    cout << "Elapsed time: " << accum << "s\n";

    if (!graph_filename.empty()) {
      create_dot_file(graph_filename,g,pr,vertex_set);
    }
    
    if (!quiet) {
      write_pageranks(g,pr,vertex_set);
    }

  }
  catch (std::exception &err) {
    cout << "Error:  " << err.what() << endl;
    return 1;
  }
  return EXIT_SUCCESS;
}
