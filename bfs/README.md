Breadth First Search
==================

Run ```make``` to generate the required executables, the run using the syntax below

Synthetic Graphs : bfs.cc
Real World File Input Graphs : bfs_real.cc

To compile bfs.cc
    ```g++ bfs.cc -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./a.out P N DEG```

e.g.
    ```./a.out 2 16384 16```


To compile bfs_real.cc
    ```g++ bfs_real.cc -lm -lrt -lpthread -O2```
  
To run with P number of threads
    ```./a.out P```
  
  It will then ask for the input file, enter:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)
  https://snap.stanford.edu/data/

The executable then outputs the time in seconds that the program took to run.
