Betweenness Centrality
======================

Run ```make``` to generate the required executables, then run using the syntax below
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./bc P N DEG```

e.g.
    ```./bc 2 16384 16```

**Notes**

The executable outputs the time in seconds that the program took to run.

bc_non_partitioned.cc contains a variant of the parallelization that updates sigma values via locks. This removes the need to initialize a sigma array for each thread separately. However the original partitioned version mostly has better performance.
