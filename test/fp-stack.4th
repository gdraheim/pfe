.( http://www.forth200x.org/tests/fp-stack.fs )

needs floating-ext

s" FLOATING-EXT" ENVIRONMENT? [IF] DROP
include test/fp-stack.fs
[ELSE]
.( no floating-ext seen, skipped execution )
[THEN]

.s
