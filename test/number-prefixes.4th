.( http://www.forth200x.org/tests/number-prefixes.fs ) cr

.( fake 'z' - char prefix is not supported ) cr
[CHAR] z VALUE 'z'

include ./tester.fs

REDEFINED-MSG OFF
include test/number-prefixes.fs

.S
