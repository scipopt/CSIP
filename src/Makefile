to_build_source:
	gcc -I ../../scipoptsuite/scip-3.2.1/src/ -I ../include/ -o csip.o -c csip.c -L../lib -Wl,-rpath=../lib/ -lscipopt
to_build_test:
	gcc -std=c99 -I ../include/ test.c  -Wl,-rpath=../src/ -Wl,-rpath=../lib -L ../src/ -lcsip -L ../lib/ -lscipopt -lm
