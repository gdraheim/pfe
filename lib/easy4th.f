\ Easy4tH.f               A 4tH to ANS Forth interface

\ Typical usage:
\ 4096 constant /string-space
\    s" easy4th.f" included

\ This is an ANS Forth program requiring:
\      1. The word NIPin the Core Ext. word set
\      2. The word /STRING in the String word set
\      3. The word D>S in the Double word set
\      4. The words MS and TIME&DATE in the Facility Ext. word set
\      5. The words [IF] and [THEN] in the Tools Ext. word set.

\ (c) Copyright 1997,9 Wil Baden, Hans Bezemer, Permission is granted by the
\ authors to use this software for any application provided this
\ copyright notice is preserved.

\ Uncomment the next line if REFILL does not function properly
\ : refill query cr true ;

\ 4tH datatypes
: ARRAY CREATE CELLS ALLOT ;
: STRING CREATE CHARS ALLOT ;
: TABLE CREATE ;

\ 4tH constants
S" MAX-N" ENVIRONMENT?
[IF]
    NEGATE 1- CONSTANT (ERROR)
[THEN]

S" MAX-N" ENVIRONMENT?
[IF]
    CONSTANT MAX-N
[THEN]

S" STACK-CELLS" ENVIRONMENT?
[IF]
    CONSTANT STACK-CELLS
[THEN]

S" /PAD" ENVIRONMENT?
[IF]
    CONSTANT /PAD
[THEN]

\ 4tH compiletime words
: [NOT] 0= ; IMMEDIATE
: [*] * ; IMMEDIATE
: [+] + ; IMMEDIATE

\ 4th wordset
: 4TH CELLS + ;
: @' @ ;
: COPY ( a b -- b ) >R DUP C@ 1+ R@ SWAP MOVE R> ;
: WAIT 1000 * MS ;

: NUMBER            ( a -- n )
    0. ROT DUP 1+ C@ [CHAR] - >R COUNT R@ IF 1 /STRING THEN >NUMBER NIP 0=
    IF D>S R> IF NEGATE THEN ELSE R> DROP 2DROP (ERROR) THEN
;

\ 4tHs C" runtime semantics emulation

( Reserve STRING-SPACE in data-space. )
CREAE STRING-SPACE       /STRING-SPACE CHARS ALLOT
VARIABLE NEXT-STRING     0 NEXT-STRING !

( caddr n addr -- )
: PLACE OVER OVER >R >R CHAR+ SWAP CHARS MOVE R> R> C! ;

( " ccc" -- caddr )
: $" [CHAR] " PARSE
    DUP 1+ NEXT-STRING @ + /STRING-SPACE >
    ABORT" String Space Exhausted. "
    STRING-SPACE NEXT-STRING @ CHARS + >R
    DUP 1+ NEXT-STRING +!
    R@ PLACE
    R>
;

\ 4tHs Random generator

( Default RNG from the C standard. 'RAND' has reasonable )
( properties, plus the advantage of being widely used. )
VARIABLE RANDSEED

32767 CONSTANT MAX-RAND

: RAND                             ( -- random )
    RANDSEED @ ( random) 1103515245 * 12345 +  DUP RANDSEED !
    16 RSHIFT  MAX-RAND AND
;

: SRAND ( n -- ) RANDSEED ! ; 1 SRAND

( Don't mumble. )
: random         ( -- n )    RAND ;
: set-random     ( n -- )    SRAND ;

( Mix 'em up. )
: randomize                     ( -- )
    TIME&DATE 12 * + 31 * + 24 * + 60 * + 60 * + set-random
;

randomize
