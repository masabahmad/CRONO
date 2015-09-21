Connected Components
=======================

Run ```make``` to generate the required executables, then run using the syntax explained below

Synthetic Graphs : connected_synthetic.cc

**Synthetic Graphs**

To compile connected_synthetic.cc
    ```g++ connected_synthetic.cc -o connected_synthetic -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./connected_synthetic P N DEG```

e.g.
    ```./connected_synthetic 2 16384 16```

**Notes**

The executables also output the time in seconds that the program took to run.
