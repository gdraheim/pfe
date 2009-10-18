\
\ testsuite.4th ---	tests to verify the proper operation
\			of the portable forth environment
\ (duz 05Aug93)
\

ONLY FORTH ALSO DEFINITIONS

CR
CR .( running "tester.fr" and "core.fr")
CR .( =================================)
CR

MARKER FORGET-TESTER/CORE
S" test/tester.fr" INCLUDED
S" test/core.fr"   INCLUDED
FORGET-TESTER/CORE

CR
CR .( running checkans, a test for words missing from the ANS word sets)
CR .( =================================================================)
CR
MARKER FORGET-CHECKANS
: WORDSETS_FS S" test/wordsets.fs" ;
S" test/checkans.fs" INCLUDED
FORGET-CHECKANS


CR
CR .( testing pfe extensions:)
EXTENSIONS ALSO FORTH
INCLUDE test/defs.4th
WAIT

\ display status and environment
SHOW-STATUS
WAIT

INCLUDE test/environ.4th
WAIT

\ test help
HELP FE.
WAIT


CR
CR .( More tests:)
CR .( ===========)
CR

INCLUDE test/stack.4th	WAIT
INCLUDE test/compare.4th	WAIT
INCLUDE test/arith.4th	WAIT
INCLUDE test/float.4th	WAIT
INCLUDE test/exceptn.4th	WAIT
INCLUDE test/string.4th	WAIT
INCLUDE test/file.4th	WAIT
INCLUDE test/intactv.4th	WAIT


CR
CR .( Do you want to try the block editor? ) KEY
BL OR CHAR n <> [IF]  USING test/benchm.blk  1 EDIT-BLOCK  [THEN]


CR
CR .( Finally some benchmarks:)
CR .( ========================)
CR

USING test/benchm.blk 1 LOAD WAIT


CR
CR .( This should be nicely formatted FORTH-source:)
CR .( =============================================)
CR

SEE DO-PRIME		WAIT


CR
CR .( System survived so far?)
CR .( Fine!)
CR

CR 
