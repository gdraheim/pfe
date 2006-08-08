(       Title:  Dynamic-Strings Examples
         File:  trydstrings.fs
      Version:  0.7.2
       Author:  David N. Williams
      License:  public domain
Starting date:  April 29, 2004
Last revision:  July 19, 2004

Version 0.7.1 was skipped.

Version 0.7.0
Last revision:  June 10, 2003

This file provides examples that use words from our
Dynamic-Strings word list.  It works with either with the pfe
DSTRINGS-EXT environment, or the ANS Forth dstrings.fs
implementation.
)

: .$stat  ( -- )
  cr ." string buffer size: " /$buf . ." bytes"
    ."    max #$frames: " max-#$frames .
 cr ." $unused:  " $unused . ." bytes   "
    ." $depth: " $depth .
    ."    $frame depth: " $frame-depth .
   base @ hex
  cr ." $buffer:  0x" $bufp@ .
  cr ." $break:   0x" $breakp@ .
    4 spaces ." $frame-break:  0x" $fbreakp@ .
  cr ." $sp:      0x" $sp@ .
    4 spaces ." $frame-sp:     0x" $fsp@ .
  cr ." $sp0:     0x" $sp0@ .
    4 spaces ." $frame-sp0:    0x" $fsp0@ .
  cr $garbage? IF ." There is some string garbage. "
    ELSE ." There is no string garbage. " THEN
  cr cat$@ IF ." Concatenation is in progress at: 0x"
      cat$@ .
    ELSE ." No concatenation is in progress." THEN
  base ! ;

: dump-strings  ( -- )
  dstrings @ ( addr)
  /$buf /$space-header + /$frame-stack + cell+ ( #bytes)
  dump .$stat ;

s" This is Bill's string." sm, 2constant bill
s" This is Marie's string." sm, 2constant marie
s" This is a string called George." sm, 2constant george

variable sys-strings dstrings @ sys-strings !

108 4  make-$space dstrings !
cr .( The system default string space pointer is in SYS-STRINGS.)
cr .( To follow what's happening, it's important to look at the)
cr .( source file, trydstrings.fs.)

\ store dynamic string
bill >$s-copy
cr
cr .( We've just copied the ANS Forth string BILL into string space)
cr .( and pushed it onto the string stack.  Note that the backward)
cr .( link points to the string stack:)
dump-strings

$variable bill$   bill$ $!

\ store external string
marie >$s $variable marie$  marie$ $!

bill$ $@ marie$ $@ george >$s 3 $frame
cr
cr .( We've made the following into a string frame [remove "]:)
cr .(   ") george type .( ")
cr .(   ") marie$ $@ $. .( ")
cr .(   ") bill$ $@ $. .( ")
cr .( Note that the first two strings are external to string space.)
cr .( The last is the same dynamic string as before, still on the)
cr .( string stack, but now the back link points outside of string)
cr .( space to BILL$, where it is also stored:)
dump-strings

cr
cr .( Using [address count] FIND-$ARG gives the following string frame)
cr .( indices:)
\ $PICK works here because the frame is at the top of the stack.
george find-$arg ( true) drop ( index) dup
  cr .(   index ) 1 .r .( :  ) ( index) $pick $. 
marie find-$arg ( true) drop ( index) dup
  cr .(   index ) 1 .r .( :  ) ( index) $pick $. 
bill find-$arg ( true) drop ( index) dup
  cr .(   index ) 1 .r .( :  ) ( index) $pick $. 
cr .( The topmost element has frame index 0.)

drop-$frame
cr
cr .( Now we've executed DROP-$FRAME, and the string and string)
cr .( frame stacks are both empty, while Bill's string remains)
cr .( stored in a string variable:)
dump-strings

cr
cr .( We're freeing the )
  /$buf . .( byte string space, and creating a larger)
cr .( one for the next test with nested arguments:)

\ This makes BILL$ invalid!
dstrings @ free ( ior) drop
204 4  make-$space dstrings !

: \n+  ( -- )  \n$ cat ;

\ test args with nesting

: reverse  $ARGS{ arg1 arg2 }
  cat" Here are the args in reverse order: " arg2 arg1 ;

: tryargs  $ARGS{ arg1 arg2 }
  cat" This is arg1: " arg1 \n+
  cat` This is arg2: ` arg2 \n+
  $over ($: arg1 arg2 arg1) $over ($: arg1 arg2 arg1 arg2)
  reverse ;

$" Hello " $` world!` tryargs ENDCAT cr $.

cr
cr .( Execute CR DUMP-STRINGS COLLECT-$GARBAGE CR DUMP-STRINGS to see)
cr .( the effect of garbage collection with no strings bound to)
cr .( variables and nothing on the string stack. )
