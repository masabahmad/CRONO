To compile range_dijk.cc
  g++ range_dijk.cc -lm -lrt -lpthread -O2
  
To run with P number of threads, N vertices, and DEG edges per vertex
  ./a.out P N DEG

e.g. ./a.out 2 16384 16


To compile range_dijk_real.cc
  g++ range_dijk_real.cc -lm -lrt -lpthread -O2
  
To run with P number of threads
  ./a.out P
  
  It will then ask for the input file, enter:
  sample.txt
