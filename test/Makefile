
TESTS = locals-ext.4th loop-test.4th \
	compare.4th defs.4th arith.4th string.4th stack.4th \
	environ.4th exceptn.4th \
	coretest.4th dbltest.4th postpone.4th

ok : $(TESTS:.4th=.ok)
chk:  $(TESTS:.4th=.out)
	for f in $(TESTS:.4th=.out) ; do mv $$f `basename $$f .out`.chk ; done

%.out : %.4th
	@ # pfe -q --canonical $< | perl -p -e '/\e/ and $$_ = "";' >$@
	@ echo 'pfe --bye --quiet $< >$@'
	@ pfe --bye --quiet $< | perl -p -e '/\e/ and $$_ = "";'  >$@
	@ if test "0" = `cat $@ | wc -l` \
	; then echo "NO OUTPUT FROM " $@ \
	; fi 

%.ok : %.out
	@ test -f $*.our || cp $*.out $*.our 
	@ if test $*.out \
	; then diff -uw $*.our $*.out >$@ \
	; else echo "" >$@ \
	; fi
	@ if test "0" != `cat $@ | wc -l` \
	; then echo $* FAIL \
	; else echo $* OK \
	; fi


clean-test:
	-rm -f *.ok *.out

clean-test-all: clean
	-rm -f *.our

