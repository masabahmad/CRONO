All Pairs Shortest Path
=======================

Run ```make``` to generate the required executables, then run using the syntax explained below

**Synthetic Graphs**

Synthetic Graphs : APSP.cc

To compile APSP.cc
    ```g++ APSP.cc -o APSP -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./APSP P N DEG```

e.g.
    ```./APSP 2 16384 16```

**Notes**

The executable then outputs the time in seconds that the program took to run.
Tested and Scales to 1024 threads.
