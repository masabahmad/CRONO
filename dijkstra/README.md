Parallel Dijkstra
=================

You can run ```make``` to create executables for each program in each subdirectory (real,synthetic,test) and use the commands outlined in each respective Markdown to run.

These programs parallelize the O(V^2) version of Dijkstra's Algorithm, given by Yen's Optimization.
Paper: J.Y.Yen, "An algorithm for finding shortest routes from all source nodes to a given destination in general networks", Quarterly of Applied Mathematics 01/1970.

**Notes**

All inputs must be in powers of 2.

The executables output the time in seconds that the program took to run.
These parallelizations can handle negative edge weights as well.
The real world graphs version has a parameter P_max, that is specified by ```cuberoot(N)*3```, which specifies the number of iterations for the outer loop.
Unlike other Graph Processing Frameworks like Ligra and Boost, we do not pre-process graphs (e.g. compression, vertex/edge reordering), and hence have a more generic implementation.

The test folder contains a testing program to check for inaccurate results.

