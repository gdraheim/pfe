.( http://www.forth200x.org/tests/number-prefixes.fs ) cr

.( fake 'z' - char prefix is not supported ) cr
[CHAR] z VALUE 'z'

.( ./ local path resolution is not supported ) cr
include test/tester.fs
ALSO LOADED DEFINITIONS
0 constant -./tester.fs
PREVIOUS

REDEFINED-MSG OFF
include test/number-prefixes.fs

.S
