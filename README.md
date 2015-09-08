
CRONO v0.9.1 : A Benchmark Suite for Multithreaded Graph Algorithms Executing on Futuristic Multicores
====================================================================

This repository contains 10 Graph Benchmarks interfaced with synthetic and real world graphs.
An in built synthetic random graph generater is included.
Real World Graphs are also interfaced as in benchmarkreal.cc files within the folders.
Can be easily run with the Graphite Simulator, and interfaced with performance counters (easyperf, coded already included and commented out).

Requirements
============

1. Linux (Tested on Ubuntu 14.04)
2. g++ 4.6 (Tested with g++ 4.7)


Features
=======
1. Ubiquitous Graph Workloads
2. Use adjacency lists, with graph files such as those in the SNAP dataset as inputs, as well as synthetic inputs
3. Most workloads scale to 256 threads, some scale upto 1024 threads as well
4. Easy to compile and use
5. Generic Graph Usage, No optimizations, such as, compression or vertex/edge reordering, done of any sort, due to additional processing time taken by these operations.

Running
=======

Checkout the Repo:
git clone https://github.com/masabahmad/CRONO

To generate executables, run ```make``` inside the CRONO directory, then run the executables for each benchmark using the syntax specified by their individual README.mds.

OR

To run, compile using the readme located in each folder

Notice
======

If you use these programs please cite:

Masab Ahmad (UConn), Farrukh Hijaz (UConn), Qingchuan Shi (UConn), Omer Khan (UConn), CRONO : A Benchmark Suite for Multithreaded Graph Algorithms Executing on Futuristic Multicores, 2015 IEEE International Symposium on Workload Characterization (IISWC), Oct 2015, Atlanta, Georgia, USA.

Contact
=======

masab.ahmad@uconn.edu
