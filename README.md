# CSIP
An optionated interface to the [SCIP](http://scip.zib.de/) solver in the C language.
A restricted subset of the features is chosen, with the goal of making SCIP more accessible to novice users and other programming languages.

The following constraint types are supported: [linear](http://scip.zib.de/doc/html/cons__linear_8h.php), [quadratic](http://scip.zib.de/doc/html/cons__quadratic_8h.php), [SOS1](http://scip.zib.de/doc/html/cons__sos1_8h.php) and [SOS2](http://scip.zib.de/doc/html/cons__sos2_8h.php).

Furthermore, users can implement a lazy constraint by implementing a single callback function.

To compile the CSIP shared library run `make`  
**Note** `make` will ask you to provide links to `libscipopt.so` and `scip/src`  

To compile and execute the test, run `make test`
