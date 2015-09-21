PageRank
========

Run ```make``` to generate executables, then use the syntax explained below for the inputs

Synthetic Graphs : pagerank.cc
Real World File Input Graphs (No Approximations) : pagerank_real.cc

A detailed explanation for Synthetic approximations are located at:
http://wwwhome.math.utwente.nl/~scheinhardtwrw/ISP2013/DeeperInsidePR.pdf

To compile pagerank.cc
    ```g++ pagerank_synthetic.cc -o pagerank_synthetic -lm -lrt -lpthread -O2```
  
To run pagerank.cc (Synthetic) with P number of threads, N vertices, and DEG edges per vertex
    ```./pagerank_synthetic P N DEG```

e.g.
    ```./pagerank_synthetic 2 16384 16```


To compile pagerank_real.cc
    ```g++ pagerank_real.cc -o pagerank_real -lm -lrt -lpthread -O2```
  
To run with P number of threads
    ```./pagerank_real P```
  
  It will then ask for the input file, enter:
  sample.txt
  OR any other file such as road networks from the SNAP datasets (e.g. A FaceBook Graph)
  https://snap.stanford.edu/data/

The test folder also compiles a test boost program that can be used to verify our PageRank programs. 

The executable then outputs the time in seconds that the program took to run.
It also outputs a file that contains the pageranks (normalized to 1).
Some very small differences in pageranks might occur due to floating point round offs within the program.
