Triangle Counting
=================

Run ```make``` to generate the required executables, then use the syntax below to run the benchmark

To compile triangle_counting.cc
    ```g++ triangle_counting.cc -o triangle_counting -lm -lrt -lpthread -O2```
  
To run with P number of threads
    ```./triangle_counting_real P <input_file>```
  
  The input file can be used as:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)
  https://snap.stanford.edu/data/

**Notes**

All inputs must be in powers of 2.

The executable then outputs the time in seconds that the program took to run, as well as the number of triangles counted.
