Triangle Counting
=================

Run ```make``` to generate the required executables, then use the syntax below to run the benchmark

Real World File Input Graphs : triangle_counting_real.cc

**Real Graphs**

To compile triangle_counting_real.cc
    ```g++ triangle_counting_real.cc -o triangle_counting_real -lm -lrt -lpthread -O2```
  
To run with P number of threads
    ```./triangle_counting_real P```
  
  It will then ask for the input file, enter:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)
  https://snap.stanford.edu/data/

**Notes**

The executable then outputs the time in seconds that the program took to run, as well as the number of triangles counted.
