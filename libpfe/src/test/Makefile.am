SUBDIRS = anstests1

_TESTS = locals-ext.4th loop-test.4th \
	compare.4th defs.4th arith.4th string.4th stack.4th \
	environ.4th exceptn.4th \
	coretest.4th dbltest.4th postpone.4th \
        queryapp.4th

nomodules = @WITHOUT_MODULES@ nomodules

CLEANFILES = *.out *.diff

ok : $(_TESTS:.4th=.ok)
chk:  $(_TESTS:.4th=.out)
	for f in $(_TESTS:.4th=.out) ; do mv $$f `basename $$f .out`.chk ; done

%.out : %.4th
	@ echo 'pfe --bye --quiet $< >$@' \
	; { pfe --bye --quiet "$<" 2>&1 } \
	| perl -p -e '/\e/ and $$_ = "";'  >$@
	@ if test "0" = `cat $@ | wc -l | sed -e "s/ *//g"` \
	; then echo "NO OUTPUT FROM " $@ \
	; fi 

%.ok : %.out
	@ test -f $*.our || cp $*.out $*.our 
	@ if test $*.out \
	; then diff -uw $*.our $*.out >$@ \
	; else echo "" >$@ \
	; fi
	@ if test "0" != `cat $@ | wc -l | sed -e "s/ *//g"` \
	; then echo $* FAIL \
	; else echo $* OK \
	; fi


clean-test:
	-rm -f *.ok *.out *.run *.gdb

clean-test-all: clean
	-rm -f *.our

noinst_DATA = run1.in.sh
_EXTRA_DIST = *.4th *.sh *.fs *.fr *.blk *.ok
include Makedist.mk

testlist:
	{ echo "EXTRA_DIST = "; (cd $(srcdir) && ls $(_EXTRA_DIST)); } \
	| sed -e "/=/!s/^/  /" -e "s/\$$/ \\\\/" > $(srcdir)/Makedist.mk \
	; echo "  \$$(NULL)" >> $(srcdir)/Makedist.mk

PFE_DIR = ../main/c
PFE_EXE = $(PFE_DIR)/pfe$(EXEEXT)
$(PFE_EXE) :
	(cd `dirname $@` && $(MAKE) `basename $@`)

SCRIPTDIR= $(srcdir)/..
PFE_CHECK_ARGS = --bye --quiet $(PFE_SCRIPTDIR_STRING) $(PFE_LIB_PATH_STRING)
PFE_LIB_PATH_STRING = --lib-path-string=$(PFE_DIR)/.libs
PFE_SCRIPTDIR_STRING = --inc-path-string=$(SCRIPTDIR)
# PFE_SCRIPTDIR_STRING = -I $(SCRIPTDIR)

WARNDIFFS = diff -uw
DIFFS = diff -u
SKIPS = queryapp

anstests:
	cd anstests1 && $(MAKE) check
	
run1.sh : run1.in.sh
	echo "#! $(SHELL)" > $@
	echo "srcdir='$(srcdir)'" >> $@
	echo "EXEEXT='$(EXEEXT)'" >> $@
	test -s $(srcdir)/run1.in.sh
	cat $(srcdir)/run1.in.sh >> $@
	- chmod +x $@
	

# check-tests will do some minor sanity checks. There may be something awefully
# wrong if these fail. check-local is also called by "make check" or distcheck
checks: check-local anstests
check-local:  $(PFE_EXE) run1.sh
	test -d $(SCRIPTDIR)/test
	$(MAKE) environ.testW.out
	$(SHELL) run1.sh $(SCRIPTDIR)/test/*.4th
	
environ.testW.out: $(srcdir)/environ.testW.ok $(srcdir)/environ.test64W.ok
	touch $@

$(srcdir)/environ.testW.ok : $(srcdir)/environ.test.ok
	cat $< | sed -e "s| *=|=|" > $@
$(srcdir)/environ.test64W.ok : $(srcdir)/environ.test64.ok
	cat $< | sed -e "s| *=|=|" > $@

# tests mean to check if the latest changes to the package made for
# some kind of difference in behaviour with respect to some checks.
# if this target is run for the first time (virgin call), it will 
# simply create local reference output, i.e. it will always succeed 
DIFFFLAGS=-bB
tests: 
	test -d $(SCRIPTDIR)/test
	@ OK="" ; BAD="" \
	; for f in $(SCRIPTDIR)/test/*.4th \
	; do : \
	;  check=`basename $$f .4th` \
	;  if test -r "$(SCRIPTDIR)/test/$$check.chk" \
	;  then echo $$check .... \
	;   $(PFE_EXE) --bye --quiet -I $(SCRIPTDIR) $$f >$$check.out \
	;   test -f $$check.our || cp $$check.out $$check.our \
	;   diffs=`diff $(DIFFFLAGS) $$check.our $$check.out | wc -l` \
	;   if test "$$diffs" -ne "0" \
	;   then BAD="$$BAD $$check" \
	;    echo $$check: " ... HAS $$diffs DIFFERENCES!!!" \
	;   else OK="$$OK $$check" \
	;   fi \
	;  fi \
	; done \
	; test -z "$$OK"  || echo "OK : $$OK" \
	; test -z "$$BAD" || echo "BAD : $$BAD" \
	; test -z "$$BAD" # let the check fail

gdb:
	LD_LIBRARY_PATH=$(PFE_DIR)/.libs/ gdb $(PFE_DIR)/.libs/pfe
#       ../pfe/pfe is a shell script that gdb will not quite like
