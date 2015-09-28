Depth First Search
==================

Run ```make``` to generate the required executables, then run using the syntax explained below

Real World File Input Graphs : dfs_real.cc

**Real Graphs**

To compile dfs_real.cc
    ```g++ dfs_real.cc -o dfs_real -lm -lrt -lpthread -O2```
  
To run with P number of threads
    ```./dfs_real P```
  
  It will then ask for the input file, enter:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)
  https://snap.stanford.edu/data/

**Notes**

All inputs must be in powers of 2.
The executable then outputs the time in seconds that the program took to run.
