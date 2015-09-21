Triangle Counting
=================

Run ```make``` to generate the required executables, then use the syntax below to run the benchmark

Synthetic Graphs : triangle_counting_synthetic.cc

**Synthetic Graphs**

To compile triangle_counting_synthetic.cc
    ```g++ triangle_counting_synthetic.cc -o triangle_counting_synthetic -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./triangle_counting_synthetic P N DEG```

e.g.
    ```./triangle_counting_synthetic 2 16384 16```

**Notes**

The executable then outputs the time in seconds that the program took to run, as well as the number of triangles counted.
