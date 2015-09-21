Breadth First Search
==================

Run ```make``` to generate the required executables, the run using the syntax below

Real World File Input Graphs : bfs_real.cc

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
