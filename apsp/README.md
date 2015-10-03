All Pairs Shortest Path
=======================

Run ```make``` to generate the required executables, then run using the syntax explained below

**Synthetic Graphs**

Synthetic Graphs : apsp.cc

To compile apsp.cc
    ```g++ apsp.cc -o apsp -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./apsp P N DEG```

e.g.
    ```./apsp 2 16384 16```

**Notes**

P, N, and DEG must be in powers of 2.
The executable outputs the time in seconds that the program took to run.
Tested and Scales to 1024 threads.
