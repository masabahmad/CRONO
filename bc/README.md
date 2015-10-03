Betweenness Centrality
======================

Run ```make``` to generate the required executables, then run using the syntax below

**Synthetic Graphs**

Synthetic Graphs : bc.cc

To compile bc.cc
    ```g++ bc.cc -o bc -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./bc P N DEG```

e.g.
    ```./bc 2 16384 16```

**Notes**

P, N, and DEG must be in powers of 2.
The executable outputs the time in seconds that the program took to run.
The test folder contains a test program to check program outputs.
