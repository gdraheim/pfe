cr .( http://www.complang.tuwien.ac.at/forth/anslocal.fs ) cr
atexit-wordlist current ! variable -parsing.fs 
definitions

include test/anslocal.fs

: test-swap { a b -- b a } ." xxx "
    b a ;

1 2 test-swap . . .s cr

cr .( done ) cr

