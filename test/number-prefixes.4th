.( http://www.forth200x.org/tests/number-prefixes.fs ) cr

.( fake 'z' - char prefix is not supported ) cr
[CHAR] z VALUE 'z'

include test/tester.fs
ALSO LOADED DEFINITIONS
0 constant -./tester.fs
PREVIOUS

include test/number-prefixes.fs

.S


