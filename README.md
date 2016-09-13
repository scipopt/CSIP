# CSIP

[![Join the chat at https://gitter.im/leethargo/CSIP](https://badges.gitter.im/leethargo/CSIP.svg)](https://gitter.im/leethargo/CSIP?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

An optionated interface to the [SCIP](http://scip.zib.de/) solver in
the C language. A restricted subset of the features is chosen, with
the goal of making SCIP more accessible to novice users and other
programming languages.

The following constraint types are supported:
[linear](http://scip.zib.de/doc/html/cons__linear_8h.php),
[quadratic](http://scip.zib.de/doc/html/cons__quadratic_8h.php),
[SOS1](http://scip.zib.de/doc/html/cons__sos1_8h.php) and
[SOS2](http://scip.zib.de/doc/html/cons__sos2_8h.php).

Furthermore, users can implement a lazy constraint by implementing a
single callback function.

## Installation

### SCIP and SoPlex

CSIP depends on the
[SCIP Optimization Suite](http://scip.zib.de/#scipoptsuite).

[Download](http://scip.zib.de/download.php?fname=scipoptsuite-3.2.1.tgz)
and extract the source files, then build the shared library
(containing SCIP and SoPlex) with

    make SHARED=true GMP=false READLINE=false ZLIB=false scipoptlib
    
which will produce a file `libscipopt.so`.

To build CSIP, set the environment variable `SCIPOPTDIR` to point to the
directory that contains the `scipoptsuite` sources. CSIP needs the library in
`${SCIPOPTDIR}/lib/scipoptlib.so` and the C header files in
`${SCIPOPTDIR}/scip-*/src/`.

### CSIP

Run `make` to build CSIP, which will produce a shared library
`libcsip.so`.

### Tests

To compile and execute the tests, run `make test`.
