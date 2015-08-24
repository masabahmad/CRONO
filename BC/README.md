Betweenness Centrality
======================

Synthetic Graphs : BC.cc

To compile BC.cc
    ```g++ BC.cc -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./a.out P N DEG```

e.g.
    ```./a.out 2 16384 16```

The executable then outputs the time in seconds that the program took to run.
