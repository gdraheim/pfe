\ This is a program that tests number conversion on a Forth system and
\ reports the results.

\ Output for various Forth systems:

0 [if]
 #9. #9 -#9. -#9 #-9. #-9 $F. $F -$F. -$F $-F. $-F %1. %1 -%1. -%1 %-1. %-1
iForth 2.1.2541:
   9  9    -   -   -9  -9  15 15    -   -  -15 -15   1  1    -   -   -1  -1
bigForth 2.1.6:
   9  9   -9  -9    -   -  15 15  -15 -15    -   -   1  1   -1  -1    -   -
gforth 0.6.9:
   9  9   -9  -9    -   -  15 15  -15 -15    -   -   1  1   -1  -1    -   -
PFE 0.33.34
   -  -    -   -    -   -  15 15  -15 -15  -15 -15   1  1   -1  -1   -1  -1
ntf/lxf (Peter Fälth)
   9  9    -   -   -9  -9  15 15    -   -  -15 -15   1  1    -   -   -1  -1
SwiftForth 3.0.11
   9  9    -   -   -9  -9  15 15    -   -  -15 -15   1  1    -   -   -1  -1
Win32Forth:
   9  9   -9  -9   -9  -9  15 15  -15 -15  -15 -15   1  1   -1  -1   -1  -1
[then]


decimal

: tests ( xt -- )
    \ xt ( c-addr u -- ) tests the string and produces corresponding output
    >r
\    s" !10" r@ execute \ not accepted by anyone in the first run
\    s" @10" r@ execute \ not accepted by anyone in the first run
    s" #9." r@ execute
    s" -#9." r@ execute
    s" #-9." r@ execute
    s" $F." r@ execute
    s" -$F." r@ execute
    s" $-F." r@ execute
    s" %1." r@ execute
    s" -%1." r@ execute
    s" %-1." r@ execute
\    s" ^10" r@ execute \ not accepted by anyone in the first run
\     s" &10" r@ execute  \ conflict (octal vs. decimal)
\     s" 0x10" r@ execute \ not widely supported
\     s" 0X10" r@ execute \ not widely supported
\     s" 10h" r@ execute  \ not widely supported
\     s" 10H" r@ execute  \ not widely supported
\    s" $f" r@ execute    \ supported by everyone but iforth
\    s" $F" r@ execute    \ supported by everyone
\    s" 'a" r@ execute \ not widely supported
\    s" 'a'" r@ execute
    r> drop ;

: input ( c-addr u -- )
    2dup space type
    1- space type ;

: interp-test ( c-addr u -- )
    dup >r ['] evaluate catch if
        2drop r> spaces ." -"
    else
        space r> .r
    then ;

: interp-d-test ( c-addr u -- )
    dup >r ['] evaluate catch if
        2drop r> spaces ." -"
    else
        space r> d.r
    then ;

: interp-tests ( c-addr u -- )
    2dup interp-d-test
    1- interp-test ;

: >num-test ( c-addr u -- )
    dup >r 0. 2swap >number if
        drop 2drop r> spaces ." -"
    else
        drop space r> d.r
    then ;

: testall ( -- )
    cr ['] input        tests  \ ."  input"
    cr ['] interp-tests tests \ ."  interpreter"
\    cr ['] >num-test   tests ."  >number" \ no prefixes accepted in first run
    cr ;
    
testall
    
    