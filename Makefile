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

CFLAGS 		= -Wall -pedantic

SCIPSRC 	= $(CSIPLIBDIR)/include
SCIPLIB 	= -lscipopt

FLAGS 		= -I$(SCIPSRC) -I$(CSIPINC)
LFLAGS 		= -L$(CSIPLIBDIR)
LINKFLAGS 	= -Wl,-rpath,$(CSIPLIBDIR)

TESTDIR 	= $(CSIPDIR)/test
TESTFLAGS 	= -I$(CSIPINC) -g
TESTLIBS 	= -lm -lcsip -lscipopt
LINKTESTFLAGS 	= $(LINKFLAGS)
LINKTESTFLAGS 	+= -Wl,-rpath,$(CSIPLIBDIR)
LTESTFLAGS 	= -L$(CSIPLIBDIR)

TESTSRC 	= $(TESTDIR)/test.c
TESTBIN 	= $(TESTDIR)/test # don't know where to put the test executable

CSIPHEADER  = $(CSIPINC)/csip.h
CSIPSRC 	= $(CSIPSRCDIR)/csip.c
CSIPOBJ 	= $(CSIPSRCDIR)/csip.o
CSIPLIB 	= $(CSIPLIBDIR)/libcsip.so

# RULES
.PHONY: all
all: 		$(CSIPLIBDIR) $(CSIPLIB)

.PHONY: clean
clean: 
	@echo "removing $(CSIPOBJ), $(CSIPLIB), $(TESTBIN)"
	@rm -f $(CSIPOBJ)
	@rm -f $(CSIPLIB)
	@rm -f $(TESTBIN)

.PHONY: clean-links
clean-links: 
	@echo "removing symlinks"
	@rm -rf $(CSIPLIBDIR)

.PHONY: test
test: 
	@make $(TESTBIN)
	$(TESTBIN)

.PHONY: links
links:
	@echo "Creating symbolic links to headers and library within SCIPOPTDIR ($(SCIPOPTDIR))."
	@bash -c 'cd lib; \
		if [ -e $(SCIPOPTDIR)/scip-*/src/scip/scip.h ] ; \
		then \
			ln -s $(SCIPOPTDIR)/scip-*/src include; \
		else \
			echo "ERROR: no SCIP headers found in SCIPOPTDIR!" ; \
			cd ..; \
			rmdir lib; \
	        exit 1; \
		fi ; \
		cd ..;\
		'
	@bash -c 'cd lib; \
		if [ -e $(SCIPOPTDIR)/lib/libscipopt.so ] ; \
		then \
			ln -s $(SCIPOPTDIR)/lib/libscipopt.so; \
		else \
			echo "ERROR: no SCIP library found in SCIPOPTDIR!" ; \
			cd ..; \
			rmdir lib; \
	        exit 1; \
		fi ; \
		cd ..;\
		'

$(CSIPLIBDIR):
	mkdir -p $@
	make links

$(CSIPLIB): $(CSIPSRC)
	gcc $(CFLAGS) $(FLAGS) -c $< $(LFLAGS) $(LINKFLAGS) $(SCIPLIB) -fPIC -o $(CSIPOBJ)
	gcc $(CFLAGS) $(CSIPOBJ) $(LFLAGS) $(LINKFLAGS) $(SCIPLIB) -fPIC -shared -o $@

$(TESTBIN): $(TESTSRC) $(CSIPLIB)
	@echo "compiling test"
	gcc $(CFLAGS) $(TESTFLAGS) $< $(LINKTESTFLAGS) $(TESTLIBS) $(LTESTFLAGS) -o $@

ASTYLEOPTS	= --style=allman --indent=spaces=4 --indent-cases --pad-oper --pad-header --unpad-paren --align-pointer=name --add-brackets --max-code-length=80

.PHONY: style
style:
	@astyle -q $(ASTYLEOPTS)  $(CSIPHEADER) $(CSIPSRC) $(TESTSRC)

.PHONY: valgrind
valgrind:
	@valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all $(TESTBIN)

