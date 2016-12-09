Single Source Shortest Path
===========================

You can run ```make``` to create executables for each program or use the following commands below

The first argument to the executable specifies whether you want to read the graph from a file (1), or generate a synthetic one internally (0).

**Input Graph from a File**

To run with P number of threads
  ```./sssp 1 P <input_file>```
  
  It will then ask for the input file, enter:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. roadNet-CA)
  https://snap.stanford.edu/data/#road

**Generate and Input using the Synthetic Graph Generator**

To run with P number of threads, N vertices, and DEG edges per vertex
  ```./sssp 0 P N DEG```

**Notes**

This version of sssp parallelizes the O(V^2) version of Dijkstra's Algorithm, given by Yen's Optimization.
Paper: J.Y.Yen, "An algorithm for finding shortest routes from all source nodes to a given destination in general networks", Quarterly of Applied Mathematics 01/1970.

SSSP has a parameter P_max that is specified by ```cuberoot(N)*3```, which represents the number of iterations for the outer loop.

The executable then outputs the time in seconds that the program took to run.

```sssp_outer.cc``` implemented a conventional parallelization of the bellman-ford algorithm. This is an iterative algorithm generally used in GPU implementations. ```sssp_outer_atomic.cc``` just replaces pthread_barriers with atomic barriers.
