Breadth First Search
==================

Run ```make``` to generate the required executables, then run using the syntax explained below

Synthetic Graphs : bfs_synthetic.cc

**Synthetic Graphs**

To compile bfs_synthetic.cc
    ```g++ bfs_synthetic.cc -o bfs_synthetic -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./bfs_synthetic P N DEG```

e.g.
    ```./bfs_synthetic 2 16384 16```

**Notes**

The executable then outputs the time in seconds that the program took to run.
