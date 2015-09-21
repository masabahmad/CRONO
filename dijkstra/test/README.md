Parallel Dijkstra
=================

**BOOST Dijkstra**

**Notes**

The executable then outputs the time in seconds that the program took to run.
The real world graphs version has a parameter P_max, that is specified by ```cuberoot(N)*3```, which specifies the number of iterations for the outer loop.
Unlike other Graph Processing Frameworks like Ligra and Boost, we do not pre-process graphs (e.g. compression, vertex/edge reordering), and hence have a more generic implementation.

The test folder contains a testing program to check for inaccurate results.

If you use these programs please cite:

Masab Ahmad (UConn), Kartik Lakhsminarasimhan (UConn), Omer Khan (UConn), Efficient Parallelization of Path Planning Workload on Single-chip Shared-memory Multicores, In Proceedings of the IEEE High Performance Extreme Computing Conference (HPEC), Sept. 2015.
