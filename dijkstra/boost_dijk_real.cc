
#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <exception>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

//#define DISTANCE_LABELS

using namespace std;
using namespace boost;

typedef adjacency_list < vecS, vecS, directedS, no_property, property < edge_weight_t, int > > graph_t;
typedef property_map<graph_t, edge_weight_t>::type weight_map_t;
typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
typedef graph_traits < graph_t >::edge_descriptor edge_descriptor;
typedef std::pair<int, int> Edge;

#define BILLION 1E9

void create_dot_file(const string &fn,graph_t &g,const vector<int> &distances,
                     const vertex_descriptor &src)
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
    dot_file << u << " -> " << v
             << " [label=\"" << get(weightmap, e) << "\"]\n";
  }

# ifdef DISTANCE_LABELS
  if (!distances.empty()) {
    graph_traits < graph_t >::vertex_iterator vi, vend;
    for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
      if (distances[*vi] != numeric_limits<int>::max()) {
        if (*vi == src) {
          dot_file << *vi << "[fillcolor=\"red\"]\n";
        } else {
          dot_file << *vi << "[label=\"" << *vi << " (" << distances[*vi] << ")\"]\n";
        }
      }
    }
  }
# endif
 
  dot_file << "}\n";
}

void add_edge_and_weight(graph_t &g,weight_map_t &w,unsigned src,unsigned dst,unsigned weight)
{
  auto e = add_edge(src,dst,g).first;
  put(w,e,weight);
}

void read_graph(const string &fn,graph_t &g,weight_map_t &w)
{
  ifstream in(fn);
  if (!in) throw runtime_error("Could not open graph file " + fn);
  
  int inter = -1;
  int previous_node = -1;

  string line;
  while (!in.eof()) {
    getline(in,line);

    if (line.empty() || line[0] == '#') continue;

    istringstream is(line);
    unsigned n0,n1;

    is >> n0 >> n1;

    if (n0 == previous_node) {
      ++inter;
    } else {
      inter = 1;
    }

    add_edge_and_weight(g,w,n0,n1,inter);

    previous_node = n0;
  }
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    cout << "Usage:  " << argv[0] << " <in-file>\n";
    return 1;
  }

  try {
    graph_t g;

    property_map<graph_t, edge_weight_t>::type w = get(edge_weight, g);

    read_graph(argv[1],g,w);

    if (num_vertices(g) == 0) {
      throw runtime_error("Graph is empty!");
    }

    vector<int> d(num_vertices(g));
    vertex_descriptor s = vertex(0, g);

    struct timespec requestStart, requestEnd;
    clock_gettime(CLOCK_REALTIME, &requestStart);

    dijkstra_shortest_paths(g,s,distance_map(&d[0]));

    clock_gettime(CLOCK_REALTIME, &requestEnd);
	  double accum = ( requestEnd.tv_sec - requestStart.tv_sec ) + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;
    cout << "Elapsed time: " << accum << "s\n";

    create_dot_file("bgraph.dot",g,d,s);
    
    cout << "distances:" << std::endl;
		ofstream myfile;
		myfile.open ("boost.txt");
    graph_traits < graph_t >::vertex_iterator vi, vend;
    for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
      if (d[*vi] != numeric_limits<int>::max()) {
        myfile << "distance(" << *vi << ") = " << d[*vi] << "\n";
      }
    }
		myfile.close();
    cout << '\n';

  }
  catch (std::exception &err) {
    cout << "Error:  " << err.what() << endl;
    return 1;
  }
  return EXIT_SUCCESS;
}
