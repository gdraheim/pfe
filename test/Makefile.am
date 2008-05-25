
_TESTS = locals-ext.4th loop-test.4th \
	compare.4th defs.4th arith.4th string.4th stack.4th \
	environ.4th exceptn.4th \
	coretest.4th dbltest.4th postpone.4th \
        queryapp.4th

nomodules = @WITHOUT_MODULES@ nomodules

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
	-rm -f *.ok *.out

clean-test-all: clean
	-rm -f *.our

_EXTRA_DIST = *.4th *.sh *.fs *.fr *.blk *.ok
include Makedist.mk

testlist:
	{ echo "EXTRA_DIST = "; (cd $(srcdir) && ls $(_EXTRA_DIST)); } \
	| sed -e "/=/!s/^/  /" -e "s/\$$/ \\\\/" > $(srcdir)/Makedist.mk \ 
	; echo "  \$$(NULL)" >> $(srcdir)/Makedist.mk

../pfe/pfe$(EXEEXT) :
	(cd ../pfe && $(MAKE) `basename $@`)

SCRIPTDIR= $(srcdir)/..
PFE = ../pfe/pfe$(EXEEXT)
PFE_CHECK_ARGS = --bye --quiet $(PFE_SCRIPTDIR_STRING) $(PFE_LIB_PATH_STRING)
PFE_LIB_PATH_STRING = --lib-path-string=../pfe/.libs
PFE_SCRIPTDIR_STRING = --inc-path-string=$(SCRIPTDIR)
# PFE_SCRIPTDIR_STRING = -I $(SCRIPTDIR)

DIFFS = diff -u
SKIPS = queryapp

# check-tests will do some minor sanity checks. There may be something awefully
# wrong if these fail. check-local is also called by "make check" or distcheck
checks: check-local
check-local:  ../pfe/pfe$(EXEEXT)
	test -d $(SCRIPTDIR)/test
	@ OK="" ; BAD="" ; SKIP="" \
	; echo "# $(PFE) $(PFE_CHECK_ARGS) ..." \
	; for f in $(SCRIPTDIR)/test/*.4th \
	; do check=`basename $$f .4th` \
	;  if echo ":$(SKIPS):" | grep ":$$check:" >/dev/null  \
	;  then echo "$$check skipped (SKIPS=$(SKIPS):)" \
	;    SKIP="$$SKIP $$check"; continue \
	;  fi \
	;  if test -r "$(SCRIPTDIR)/test/$$check.test.ok" \
	;  then echo -n test/$$check \
	;   if test -f $(SCRIPTDIR)/test/$$check.txt \
	;    then cat $(SCRIPTDIR)/test/$$check.txt \
	|    cat| $(PFE) $(PFE_CHECK_ARGS) $$f >$$check.out 2>&1 \
	;    else $(PFE) $(PFE_CHECK_ARGS) $$f >$$check.out 2>&1 ; fi \
	;   ok="BAD" \
	;   for logfile in $(SCRIPTDIR)/test/$$check.*.ok; do : \
	;     if test -f "$$logfile"; then : \
	;       log=`basename "$$logfile" .ok` \
	;       $(DIFFS) "$$logfile" $$check.out > $$log.diff \
	;       if test -s $$log.diff; then : \
	;       else ok="$$log.ok"; break \
	;       fi \
	;      fi \
	;   done \
	;   echo ... "$$ok" \
	;   if test "$$ok" = "BAD"; then BAD="$$BAD $$check" \
	;   else OK="$$OK $$check"; fi \
	;  elif test -r "$(SCRIPTDIR)/test/$$check.sh" \
	;  then echo test/$$check .... \
	;   if test "$(SED)" = ":" ; then SKIP="$$SKIP $$check" \
	;   else if test -f $(SCRIPTDIR)/test/$$check.txt \
	;    then cat $(SCRIPTDIR)/test/$$check.txt \
	|    cat| $(PFE) $(PFE_CHECK_ARGS) $$f >$$check.out \
	;    else $(PFE) $(PFE_CHECK_ARGS) $$f >$$check.out ; fi \
	;   if test `cat $$check.out | wc -l ` = "0" \
	;   then if grep "no .* wordset" $$check.out \
        ;        then SKIP="$$SKIP $$check" \
        ;        else BAD="$$BAD $$check" ; echo "... nothing" ; fi else : \
	;   if $(SHELL) "$(SCRIPTDIR)/test/$$check.sh" $$check.out \
	;   then OK="$$OK $$check" ; elif test "$$?" = "11" \
	;   then SKIP="$$SKIP $$check" ; else BAD="$$BAD $$check" \
	;   fi fi fi \
	;  elif test -r "$(SCRIPTDIR)/test/$$check.pl" \
	;  then echo test/$$check .... \
	;   if test "$(PERL)" = ":" ; then SKIP="$$SKIP $$check" \
	;   else if test -f $(SCRIPTDIR)/test/$$check.txt \
	;    then cat $(SCRIPTDIR)/test/$$check.txt \
	|    cat| $(PFE) $(PFE_CHECK_ARGS) $$f >$$check.out \
	;    else $(PFE) $(PFE_CHECK_ARGS) $$f >$$check.out ; fi \
	;   if test `cat $$check.out | wc -l ` = "0" \
	;   then if grep "no .* wordset" $$check.out \
        ;        then SKIP="$$SKIP $$check" \
        ;        else BAD="$$BAD $$check" ; echo "... nothing" ; fi else : \
	;   if $(PERL) "$(SCRIPTDIR)/test/$$check.pl" $$check.out \
	;   then OK="$$OK $$check" ; elif test "$$?" = "11" \
	;   then SKIP="$$SKIP $$check" ; else BAD="$$BAD $$check" \
	;   fi fi fi \
	;  fi \
	; done \
	; test -z "$$OK"   || echo "OK   : $$OK" \
	; test -z "$$BAD"  || echo "BAD  : $$BAD" \
	; test -z "$$SKIP" || echo "SKIP : $$SKIP" \
	; test -z "$$BAD" # let the check fail

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
	;   $(PFE) --bye --quiet -I $(SCRIPTDIR) $$f >$$check.out \
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
	LD_LIBRARY_PATH=../pfe/.libs/ gdb ../pfe/.libs/pfe
#       ../pfe/pfe is a shell script that gdb will not quite like
