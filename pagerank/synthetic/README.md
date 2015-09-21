PageRank
========

Run ```make``` to generate executables, then use the syntax explained below for the inputs

Synthetic Graphs : pagerank_synthetic.cc

**Synthetic Graphs**

A detailed explanation for Synthetic approximations are located at:
http://wwwhome.math.utwente.nl/~scheinhardtwrw/ISP2013/DeeperInsidePR.pdf

To compile pagerank.cc
    ```g++ pagerank_synthetic.cc -o pagerank_synthetic -lm -lrt -lpthread -O2```
  
To run pagerank.cc (Synthetic) with P number of threads, N vertices, and DEG edges per vertex
    ```./pagerank_synthetic P N DEG```

e.g.
    ```./pagerank_synthetic 2 16384 16```

**Notes**

The executable then outputs the time in seconds that the program took to run.
It also outputs a file that contains the pageranks (normalized to 1).
Some very small differences in pageranks might occur due to floating point round offs within the program.
