: dump-strings  ( -- )
  dstrings @ /$space /$space-header + /frame-stack + cell+ dump
  cr ." $unused:  " $unused . ." bytes"
    4 spaces ." max #frames:   " #frames .
  base @ hex
  cr ." $buffer:  0x" $buffer .
  cr ." $break:   0x" $break .
    4 spaces ." $frame-break:  0x" $fbreak .
  cr ." $sp:      0x" $sp .
    4 spaces ." $frame-sp:     0x" $fsp .
  cr ." $sp0:     0x" $sp0 .
    4 spaces ." $frame-sp0:    0x" $fsp0 .
  base ! ;

s" This is Bill's string." s, 2constant bill
s" This is Marie's string." s, 2constant marie
s" This is a string called George." s, 2constant george

variable sys-strings dstrings @ sys-strings !

108 4  make-$space dstrings !
cr .( The system default string space pointer is in SYS-STRINGS.)
cr .( The current string space allows 4 frames and has)
cr .( a ) /$space . .( byte string buffer.)

\ store dynamic string
bill >$s-copy
dump-strings
$variable bill$  bill$ $!

\ store external string
marie >$s $variable marie$  marie$ $!

bill$ $@ marie$ $@ george >$s 3 $frame
cr .( We've made the following into a string frame:)
cr bill$ $@ $.
cr marie$ $@ $.
cr george type
dump-strings

bill find-arg cr .( bill has index ) drop . 
marie find-arg cr .( marie has index ) drop .
george find-arg cr .( george has index ) drop . 

drop-$frame
cr .( We've dropped the string frame:)
dump-strings

cr .( We've freed the )
  /$space . .( byte string space, and created a larger)
cr .( one for the next test with nested arguments:)

dstrings @ free
204 4  make-$space dstrings !

: \n+  ( -- )  \n$ cat ;

\ test args with nesting

: reverse  args{ arg1 arg2 }
  m" Here are the args in reverse order: " arg2 arg1 ;

: tryargs  args{ arg1 arg2 }
  m" This is arg1: " arg1 \n+
  m" This is arg2: " arg2 \n+
  $over ($: arg1 arg2 arg1) $over ($: arg1 arg2 arg1 arg2)
  reverse ;

$" Hello " $" world!" tryargs endcat cr $.




