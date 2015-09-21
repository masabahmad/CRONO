Depth First Search
==================

Run ```make``` to generate the required executables, then run using the syntax explained below

Synthetic Graphs : dfs_synthetic.cc

**Synthetic Graphs**

To compile dfs_synthetic.cc
    ```g++ dfs_synthetic.cc -o dfs_synthetic -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./dfs_synthetic P N DEG```

e.g.
    ```./dfs_synthetic 2 16384 16```

**Notes**

The executable then outputs the time in seconds that the program took to run.
