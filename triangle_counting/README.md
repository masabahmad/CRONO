Triangle Counting
=================

Run ```make``` to generate the required executables, then use the syntax below to run the benchmark

Synthetic Graphs : triangle_counting.cc
Real World File Input Graphs : triangle_counting_real.cc

To compile triangle_counting.cc
    ```g++ triangle_counting.cc -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
    ```./a.out P N DEG```

e.g.
    ```./a.out 2 16384 16```


To compile triangle_counting_real.cc
    ```g++ triangle_counting_real.cc -lm -lrt -lpthread -O2```
  
To run with P number of threads
    ```./a.out P```
  
  It will then ask for the input file, enter:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)
  https://snap.stanford.edu/data/

Test program for real graphs : tri_cnt_real_test.cc
Run this test program with an input graph to get a triangle count, which can be compared with the parallel version.

The executable then outputs the time in seconds that the program took to run, as well as the number of triangles counted.
