Synthetic Graphs : range_dijk.cc
Real World File Input Graphs : range_dijk_real.cc

To compile range_dijk.cc
  ```g++ range_dijk.cc -lm -lrt -lpthread -O2```
  
To run with P number of threads, N vertices, and DEG edges per vertex
  ```./a.out P N DEG```

e.g. ```./a.out 2 16384 16```


To compile range_dijk_real.cc
  ```g++ range_dijk_real.cc -lm -lrt -lpthread -O2```
  
To run with P number of threads
  ```./a.out P```
  
  It will then ask for the input file, enter:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. roadNet-CA)
  https://snap.stanford.edu/data/#road

The executable then outputs the time in seconds that the program took to run.
