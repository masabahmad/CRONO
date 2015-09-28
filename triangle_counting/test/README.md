Triangle Counting
=================

Run ```make``` to generate the required executables, then use the syntax below to run the benchmark

Real World File Input Graphs : tri_cnt_test.cc

**Real Graphs**

To compile tri_cnt_test.cc
    ```g++ tri_cnt_test.cc -o tri_cnt_test -lm -lrt -lpthread -O2```
  
To run with P number of threads
    ```./tri_cnt_test P```
  
  It will then ask for the input file, enter:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)
  https://snap.stanford.edu/data/

**Notes**

All inputs must be in powers of 2.

The executable then outputs the time in seconds that the program took to run, as well as the number of triangles counted.
