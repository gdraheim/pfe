\ This is a program that tests number conversion on a Forth system and
\ reports the results.

\ Output for various Forth systems:

\ iforth 2.1.2541:
\  !10 @10 #10 $10 %10 ^10 &10 0x10 0X10 10h 10H $f $F 'a 'a' input
\    -   -  10  16   2   -   -    -    -   -   -  - 15  -  97 interpreter
\    -   -   -   -   -   -   -    -    -   -   -  -  -  -   - >number
\ bigFORTH rev. 2.1.6
\  !10 @10 #10 $10 %10 ^10 &10 0x10 0X10 10h 10H $f $F 'a 'a' input
\    -   -  10  16   2   -  10    -    -   -   - 15 15 97 24871 interpreter
\    -   -   -   -   -   -   -    -    -   -   -  -  -  -   - >number
\ PFE 0.33.34
\  !10 @10 #10 $10 %10 ^10 &10 0x10 0X10 10h 10H $f $F 'a 'a' input
\    -   -   -  16   2   -  10   16   16   -   - 15 15  -   - interpreter
\    -   -   -   -   -   -   -    -    -   -   -  -  -  -   - >number
\ Gforth-0.6.2:
\  !10 @10 #10 $10 %10 ^10 &10 0x10 0X10 10h 10H $f $F 'a 'a' input
\    -   -   -  16   2   -  10    -    -   -   - 15 15 97 24871 interpreter
\    -   -   -   -   -   -   -    -    -   -   -  -  -  -   - >number
\ Gforth 0.6.9:
\  !10 @10 #10 $10 %10 ^10 &10 0x10 0X10 10h 10H $f $F 'a 'a' input
\    -   -  10  16   2   -  10   16   16   -   - 15 15 97  97 interpreter
\    -   -   -   -   -   -   -    -    -   -   -  -  -  -   - >number
\ Win32Forth version 4.xxx
\  !10 @10 #10 $10 %10 ^10 &10 0x10 0X10 10h 10H $f $F 'a 'a' input
\    -   -   -  16   -   -   -   16   16   -   - 15 15  -  97 interpreter
\    -   -   -   -   -   -   -    -    -   -   -  -  -  -   - >number
\ Win32Forth version 6.xx and Win32Forth-STC version 0.02.xx
\  !10 @10 #10 $10 %10 ^10 &10 0x10 0X10 10h 10H $f $F 'a 'a' input
\    -   -  10  16   2   -  10   16   16  16  16 15 15  -  97 interpreter
\    -   -   -   -   -   -   -    -    -   -   -  -  -  -   - >number
\ SwiftForth
\  !10 @10 #10 $10 %10 ^10 &10 0x10 0X10 10h 10H $f $F 'a 'a' input
\    -   -  10  16   2   -   8    -    -   -   - 15 15  -   - interpreter
\    -   -   -   -   -   -   -    -    -   -   -  -  -  -   - >number
\ VFX Forth 4.0.2 build 2428
\  !10 @10 #10 $10 %10 ^10 &10 0x10 0X10 10h 10H $f $F 'a 'a' input
\    -   -  10  16   2   -   -   16   16  16  16 15 15  -   - interpreter
\    -   -   -   -   -   -   -    -    -   -   -  -  -  -   - >number
\ ntf/lxf (Peter Fälth)
\  !10 @10 #10 $10 %10 ^10 &10 0x10 0X10 10h 10H $f $F 'a 'a' input
\    -   -  10  16   2   -   8    -    -   -   - 15 15  -   - interpreter
\    -   -   -   -   -   -   -    -    -   -   -  -  -  -   - >number



decimal

: tests ( xt -- )
    \ xt ( c-addr u -- ) tests the string and produces corresponding output
    >r
    s" !10" r@ execute
    s" @10" r@ execute
    s" #10" r@ execute
    s" $10" r@ execute
    s" %10" r@ execute
    s" ^10" r@ execute
    s" &10" r@ execute
    s" 0x10" r@ execute
    s" 0X10" r@ execute
    s" 10h" r@ execute
    s" 10H" r@ execute
    s" $f" r@ execute
    s" $F" r@ execute
    s" 'a" r@ execute
    s" 'a'" r@ execute
    r> drop ;

: input ( c-addr u -- )
    space type ;

: interp-test ( c-addr u -- )
    dup >r ['] evaluate catch if
        2drop r> spaces ." -"
    else
        space r> .r
    then ;

: >num-test ( c-addr u -- )
    dup >r 0. 2swap >number if
        drop 2drop r> spaces ." -"
    else
        drop space r> d.r
    then ;

: testall ( -- )
    cr ['] input      tests ."  input"
    cr ['] interp-test tests ."  interpreter"
    cr ['] >num-test  tests ."  >number"
    cr ;
    
testall
    
    