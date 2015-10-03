
CRONO v0.9 : A Benchmark Suite for Multithreaded Graph Algorithms Executing on Futuristic Multicores
====================================================================

[![build status](https://travis-ci.org/masabahmad/CRONO.svg?branch=master)](https://travis-ci.org/masabahmad/CRONO)

This Pre-Release repository contains 10 Graph Benchmarks interfaced with synthetic and real world graphs.
An in built synthetic random graph generater is included.
Real World Graphs are also interfaced as in benchmarkreal.cc files within the folders.
Can be easily run with the Graphite Simulator, and interfaced with performance counters (easyperf, code already included and commented out).

Requirements
============

1. Linux (Tested on Ubuntu 14.04)
2. g++ 4.6 (Tested with g++ 4.7)
3. Boost Graph Library 1.55.0
4. The ```pthread``` Library

To install the Boost Graph Library, do:
```sudo apt-get install libboost-thread-dev libboost-system-dev libboost-test-dev```

Features
=======
1. Ubiquitous Graph Workloads
2. Use adjacency lists, with graph files such as those in the SNAP dataset as inputs, as well as synthetic inputs
3. Most workloads scale to 256 threads, some scale upto 1024 threads as well
4. Easy to compile and use
5. All benchmarks contain test programs to check for inaccurcies.
6. Generic Graph Usage, No Optimizations/Pre-Processing, such as, Graph Compression or Vertex/Edge reordering, done of any sort, due to additional overheads taken by these operations.

Running
=======

Checkout the Repo:
```git clone https://github.com/masabahmad/CRONO```

To generate executables, run ```make``` inside the CRONO directory, then run the executables for each benchmark using the syntax specified by their individual README.mds.

OR

To run, compile using the readme located in each folder

Notice
======

If you use these programs please cite:

Masab Ahmad (UConn), Farrukh Hijaz (UConn), Qingchuan Shi (UConn), Omer Khan (UConn), CRONO : A Benchmark Suite for Multithreaded Graph Algorithms Executing on Futuristic Multicores, 2015 IEEE International Symposium on Workload Characterization (IISWC), Oct 2015, Atlanta, Georgia, USA.

Paper Located At:
http://www.engr.uconn.edu/~omer.khan/pubs/crono-iiswc15.pdf

Contact
=======

masab.ahmad@uconn.edu

Acknowledgements
================

We thank Hamza Omar (UConn) and Brian Kahne (Freescale Semiconductor) for their help in testing and running these programs.
