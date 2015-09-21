Connected Components
=======================

Run ```make``` to generate the required executables, then run using the syntax explained below

Synthetic Graphs : connected_synthetic.cc
Real World Graphs : connected_real.cc

**Synthetic Graphs**

To compile connected_synthetic.cc
    ```g++ connected_synthetic.cc -o connected_synthetic -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./connected_synthetic P N DEG```

e.g.
    ```./connected_synthetic 2 16384 16```

**Real Graphs**

For real graphs, similarly compile connected_real.cc, then run using the input file, or any other file from the SNAP Suite,
```./connected_real 1 sample.txt```

**Notes**

The executables also output the time in seconds that the program took to run.

The test folder includes a checker program, that checks for accuracy.
