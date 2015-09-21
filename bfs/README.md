Breadth First Search
==================

Run ```make``` to generate the required executables, the run using the syntax below

Synthetic Graphs : bfs_synthetic.cc
Real World File Input Graphs : bfs_real.cc

**Synthetic Graphs**

To compile bfs_synthetic.cc
    ```g++ bfs_synthetic.cc -o bfs_synthetic -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./bfs_synthetic P N DEG```

e.g.
    ```./bfs_synthetic 2 16384 16```

**Real Graphs**

To compile bfs_real.cc
    ```g++ bfs_real.cc -o bfs_real -lm -lrt -lpthread -O2```
  
To run with P number of threads
    ```./bfs_real P```
  
  It will then ask for the input file, enter:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)
  https://snap.stanford.edu/data/

**Notes**

The executable then outputs the time in seconds that the program took to run.

The test folder contains a file that tests the bfs output.
