Parallel Dijkstra
=================

You can run ```make``` to create executables for each program or use the following commands below

To compile sssp.cc
  ```g++ sssp.cc -o sssp -lm -lrt -lpthread -O2```
  
To run with P number of threads
  ```./sssp P```
  
  It will then ask for the input file, enter:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. roadNet-CA)
  https://snap.stanford.edu/data/#road

**Notes**

All inputs must be in powers of 2.

The executable then outputs the time in seconds that the program took to run.
This parallelization can handle negative edge weights as well.
The real world graphs version has a parameter P_max, that is specified by ```cuberoot(N)*3```, which specifies the number of iterations for the outer loop.
Another parameter, ```range```, specifies the amount of vertices that can be worked on in an iteration. Currently, its set to multiply by DEG at each iteration. 
Unlike other Graph Processing Frameworks like Ligra and Boost, we do not pre-process graphs (e.g. compression, vertex/edge reordering), and hence have a more generic implementation.

