# nbhashtable
Our implementation of a Non-blocking Hashtable [as described by Chris Purcell and Tim Harris.](http://link.springer.com/chapter/10.1007%2F11561927_10).

To build our project, simply run "make" with no targets. Build will automatically be placed into build/ directory as "nbhashtable". 

By default, "make" alone just builds the executables for our two tests for correctness and execution time. We've also created a make target for running the same tests we wrote about in our report. The target for this is "make executionTimeTest". With the default settings, it does take a while! You can edit the testing parameters in executiontime.cpp.