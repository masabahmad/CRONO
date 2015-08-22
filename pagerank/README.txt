Synthetic Graphs : pagerank.cc
Real World File Input Graphs (No Approximations) : pagerank_real.cc
A detailed explanation for Synthetic approximations are located at:
http://wwwhome.math.utwente.nl/~scheinhardtwrw/ISP2013/DeeperInsidePR.pdf

To compile pagerank.cc
  g++ pagerank.cc -lm -lrt -lpthread -O2
  
To run with P number of threads, N vertices, and DEG edges per vertex
  ./a.out P N DEG

e.g. ./a.out 2 16384 16


To compile range_dijk_real.cc
  g++ pagerank_real.cc -lm -lrt -lpthread -O2
  
To run with P number of threads
  ./a.out P
  
  It will then ask for the input file, enter:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)
  https://snap.stanford.edu/data/

The executable then outputs the time in seconds that the program took to run.
