# CSIP

An opinionated interface to the [SCIP](http://scip.zib.de/) solver in
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

## Update (March 2019)

This package was initially developed as a crutch for [SCIP.jl](https://github.com/SCIP-Interfaces/SCIP.jl), but is no longer used there. From now on, CSIP can be consideren unmaintained.

## Installation

### SCIP and SoPlex

CSIP depends on the [SCIP Optimization Suite](http://scip.zib.de/#scipoptsuite).
Starting with release 0.5.0, **CSIP only supports SCIP Optimization Suite
5.0.0** or newer.

[Download](http://scip.zib.de/download.php?fname=scipoptsuite-5.0.0.tgz) the
SCIP Optimization Suite and extract the source files. Now choose a destination
path for the installation and set the environment variable `SCIPOPTDIR` there.
Build the shared library (containing SCIP and SoPlex) using `cmake` via

    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=$SCIPOPTDIR ..
    make
    make install

### CSIP

Run `make` to build CSIP, which will produce a shared library
`libcsip.so`.

### Tests

To compile and execute the tests, run `make test`.
