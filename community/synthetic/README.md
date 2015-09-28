Community Detection
=======================

Run ```make``` to generate the required executables, then run using the syntax explained below

Synthetic Graphs : community_synthetic.cc

**Synthetic Graphs**

To compile community_synthetic.cc
    ```g++ community_synthetic.cc -o community_synthetic -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./community_synthetic P N DEG```

e.g.
    ```./community_synthetic 2 16384 16```

**Notes**

All inputs must be in powers of 2.
The executables also output the time in seconds that the program took to run.
