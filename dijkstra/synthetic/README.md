Parallel Dijkstra
=================

Synthetic Graphs : dijk_synthetic.cc

You can run ```make``` to create executables for each program or use the following commands below

**Synthetic Graphs**

To compile dijk_synthetic.cc
  ```g++ dijk_synthetic.cc -o dijk_synthetic -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
  ```./dijk_synthetic P N DEG```

e.g. ```./dijk_synthetic 2 16384 16```

**Notes**

All inputs must be in powers of 2.

The executable then outputs the time in seconds that the program took to run.
This parallelization can handle negative edge weights as well.
Another parameter, ```range```, specifies the amount of vertices that can be worked on in an iteration. Currently, its set to multiply by DEG at each iteration.
Unlike other Graph Processing Frameworks like Ligra and Boost, we do not pre-process graphs (e.g. compression, vertex/edge reordering), and hence have a more generic implementation.
