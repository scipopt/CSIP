# is not easy to make makefiles :(
# it is so ugly...
# always needed reminder:
# rules/targets are of the form
# target: dependency1 dependency2 ... dependencyN
# 		command_to_execute
#               @other_command_to_execute
# if a command starts with @, it doesn't print the command, it just executes it
# $@ is the target's name
# $< is the first dependency
# $^ are all dependencies
# $? are all dependencies that are newer than target

# DEFINITIONS
CSIPDIR 	= $(realpath .)

CSIPSRCDIR 	= $(CSIPDIR)/src
CSIPINC 	= $(CSIPDIR)/include
CSIPLIBDIR 	= $(CSIPDIR)/lib

CFLAGS 		= -std=c99 -Wall -pedantic

SCIPSRC 	= $(CSIPLIBDIR)/include
SCIPLIB 	= -lscipopt

FLAGS 		= -I$(SCIPSRC) -I$(CSIPINC)
LFLAGS 		= -L$(CSIPLIBDIR)
LINKFLAGS 	= -Wl,-rpath=$(CSIPLIBDIR)

TESTDIR 	= $(CSIPDIR)/test
TESTFLAGS 	= -I$(CSIPINC)
TESTLIBS 	= -lm -lcsip -lscipopt
LINKTESTFLAGS 	= $(LINKFLAGS)
LINKTESTFLAGS 	+= -Wl,-rpath=$(CSIPLIBDIR)
LTESTFLAGS 	= -L$(CSIPLIBDIR)

TESTSRC 	= $(TESTDIR)/test.c
TESTBIN 	= $(TESTDIR)/test # don't know where to put the test executable

CSIPHEADER  = $(CSIPINC)/csip.h
CSIPSRC 	= $(CSIPSRCDIR)/csip.c
CSIPLIB 	= $(CSIPLIBDIR)/libcsip.so

# RULES
.PHONY: all
all: 		$(CSIPLIBDIR) $(CSIPLIB)

.PHONY: clean
clean: 
	@echo "removing $(CSIPLIB), $(TESTBIN)"
	@rm -f $(CSIPLIB)
	@rm -f $(TESTBIN)

.PHONY: test
test: 
	@make $(TESTBIN)
	$(TESTBIN)

.PHONY: links
links: 
	@echo "please create links to scip/src and libscipopt; if you get them wrong, call make links to try again. \n\
	Links will be created at $(CSIPLIBDIR). Press return if not needed"
	@echo "Ok, first scip/src: it should be something like path_to_scipoptsuite/scip-VERSION/src"
	@bash -ec 'cd lib; \
		eval read -e TARGET; \
		if test "$$TARGET" != "" ; \
		then \
			echo "Creating link: ln -s $$TARGET"; \
			ln -s $$TARGET include; \
		else \
			echo "skipped creation of softlink" ; \
		fi ; \
		cd ..;\
		'
	@echo "Now libscipopt.so: it should be something like path_to_scipoptsuite/lib/libscipopt.so"
	@bash -ec 'cd lib; \
		eval read -e TARGET; \
		if test "$$TARGET" != "" ; \
		then \
			echo "Creating link: ln -s $$TARGET"; \
			ln -s $$TARGET libscipopt.so; \
		else \
			echo "skipped creation of softlink" ; \
		fi ; \
		cd ..;\
		'

$(CSIPLIBDIR):
	mkdir -p $@
	make links

$(CSIPLIB): $(CSIPSRC)
	gcc $(CFLAGS) $(FLAGS) -c $< $(LFLAGS) $(LINKFLAGS) $(SCIPLIB) -fPIC -o $@
	@#gcc -I $(SCIPSRC) -I ../include/ -c csip.c -L../lib -Wl,-rpath=../lib/ -lscipopt -fPIC -o libcsip.so

$(TESTBIN): $(TESTSRC) $(CSIPLIB)
	@echo "compiling test"
	gcc $(CFLAGS) $(TESTFLAGS) $< $(LINKTESTFLAGS) $(TESTLIBS) $(LTESTFLAGS) -o $@
	@#gcc -std=c99 -I ../include/ test.c  -Wl,-rpath=../src/ -Wl,-rpath=../lib -L ../src/ -lcsip -L ../lib/ -lscipopt -lm

ASTYLEOPTS	= --style=allman --indent=spaces=4 --indent-cases --pad-oper --pad-header --unpad-paren --align-pointer=name --add-brackets --max-code-length=80

.PHONY: style
style:
	@astyle -q $(ASTYLEOPTS)  $(CSIPHEADER) $(CSIPSRC) $(TESTSRC)
