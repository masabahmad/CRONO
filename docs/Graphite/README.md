Interfacing with the Graphite Simulator
=======================================

This folder is made to interface and run this benchmark with the Graphite Simulator.

Simply copy a benchmark folder (e.g. apsp) into the directory
/GraphiteHome/tests/benchmarks/

1. Copy and modify the Makefile as needed

2. Uncomment the simulator header file: 
   ```#include "carbon_user.h"  /*For the Graphite Simulator*/```
3. Uncomment enabling simulator models ```CarbonEnableModels();```
4. and disabling them ```CarbonDisableModels();```

5. and run ```make``` in the benchmark folder

   OR run ```make apsp_bench_test``` in the Graphite Home directory

**Notes**

Some versions of the Graphite Simulator may require the benchmark name and credentials to be added to the Makefile.tests file.
