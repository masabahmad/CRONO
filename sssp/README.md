Single Source Shortest Path
===========================

You can run ```make``` to create executables for each program or use the following commands below
  
To run with P number of threads
  ```./sssp P```
  
  It will then ask for the input file, enter:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. roadNet-CA)
  https://snap.stanford.edu/data/#road

**Notes**

Input threads must be in powers of 2.

This version of sssp parallelizes the O(V^2) version of Dijkstra's Algorithm, given by Yen's Optimization.
Paper: J.Y.Yen, "An algorithm for finding shortest routes from all source nodes to a given destination in general networks", Quarterly of Applied Mathematics 01/1970.

This parallelization can handle negative edge weights as well.

The benchmark version has a parameter P_max, that is specified by ```cuberoot(N)*3```, which represents the number of iterations for the outer loop.
Another parameter, ```range```, specifies the amount of vertices that can be worked on in an iteration. Currently, its set to multiply by DEG at each iteration. 

The executable then outputs the time in seconds that the program took to run.
