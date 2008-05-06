
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

# EXTRA_DIST = *.4th *.sh *.fs *.fr *.blk
EXTRA_DIST = \
answords.fs       fib.fs               search-order.4th \
arith.4th         file.4th             search-order.fs \
arith.sh          file.sh              search-order.sh \
benchm.blk        float.4th            sieve.fs \
bubble.fs         float.sh             sievem.fs \
checkans.4th      forth-signal.4th     siev.fs \
checkans.fs       fpnostack.4th        sokoban.4th \
checkans.sh       fpnostack.sh         sokoban.fs \
compare.4th       fp-stack.4th         speedwhile.fs \
compare.sh        fp-stack.fs          stack.4th \
compspeed.fs      hashvocdistri.fs     stack.sh \
core.fr           intactv.4th          string.4th \
coretest.4th      locals-ext.4th       string.sh \
coretest.fs       loop-test.4th        suite.4th \
coretest.sh       loop-test.sh         tester.fr \
dbltest.4th       matrix.fs            tester.fs \
dbltest.fs        mini-oof.fs          test-num-prefixes2.4th \
deferred.fs       moof-exm.fs          test-num-prefixes2.fs \
deferred-x.4th    moofglos.fs          test-num-prefixes.4th \
deferred-x.fs     number-prefixes.4th  test-num-prefixes.fs \
defs.4th          number-prefixes.fs   trydstrings.fs \
defs.sh           parse-name.4th       tt.4th \
dstester.fs       parse-name.fs        tt.fs \
dstrings-test.fs  postpone.4th         wordsets.4th \
environ.4th       postpone.fs          wordsets.fs \
environ.sh        postpone.sh          wordsets.sh \
exceptn.4th       queryapp.4th


../pfe/pfe$(EXEEXT) :
	(cd ../pfe && $(MAKE) `basename $@`)

SCRIPTDIR= $(srcdir)/..
PFE = ../pfe/pfe$(EXEEXT)
PFE_CHECK_ARGS = --bye --quiet $(PFE_SCRIPTDIR_STRING) $(PFE_LIB_PATH_STRING)
PFE_LIB_PATH_STRING = --lib-path-string=../pfe/.libs
PFE_SCRIPTDIR_STRING = --inc-path-string=$(SCRIPTDIR)
# PFE_SCRIPTDIR_STRING = -I $(SCRIPTDIR)

# check-tests will do some minor sanity checks. There may be something awefully
# wrong if these fail. check-local is also called by "make check" or distcheck
check-local:  ../pfe/pfe$(EXEEXT)
	test -d $(SCRIPTDIR)/test
	@ OK="" ; BAD="" ; SKIP="" \
	; echo "# $(PFE) $(PFE_CHECK_ARGS) ..." \
	; for f in $(SCRIPTDIR)/test/*.4th \
	; do : \
	;  check=`basename $$f .4th` \
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
	;       diff -u $$check.out "$$logfile" > $$log.diff \
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
