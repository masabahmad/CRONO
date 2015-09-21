Depth First Search
==================

Run ```make``` to generate the required executables, then run using the syntax explained below

Synthetic Graphs : dfs_synthetic.cc
Real World File Input Graphs : dfs_real.cc

**Synthetic Graphs**

To compile dfs_synthetic.cc
    ```g++ dfs_synthetic.cc -o dfs_synthetic -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./dfs_synthetic P N DEG```

e.g.
    ```./dfs_synthetic 2 16384 16```

**Real Graphs**

To compile dfs_real.cc
    ```g++ dfs_real.cc -o dfs_real -lm -lrt -lpthread -O2```
  
To run with P number of threads
    ```./dfs_real P```
  
  It will then ask for the input file, enter:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)
  https://snap.stanford.edu/data/

**Notes**

The executable then outputs the time in seconds that the program took to run.

The test folder contains a program that checks for the correct dfs output.
