All Pairs Shortest Path
=======================

Synthetic Graphs : APSP.cc

To compile APSP.cc
    ```g++ APSP.cc -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./a.out P N DEG```

e.g.
    ```./a.out 2 16384 16```

The executable then outputs the time in seconds that the program took to run.
Tested and Scales to 1024 threads.
