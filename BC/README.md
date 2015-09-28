Betweenness Centrality
======================

Run ```make``` to generate the required executables, then run using the syntax below

**Synthetic Graphs**

Synthetic Graphs : BC.cc

To compile BC.cc
    ```g++ BC.cc -o BC -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./BC P N DEG```

e.g.
    ```./BC 2 16384 16```

**Notes**

P, N, and DEG must be in powers of 2.
The executable outputs the time in seconds that the program took to run.
The test folder contains a test program to check program outputs.
