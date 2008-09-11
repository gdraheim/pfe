(       Title:  Dynamic-Strings Word Set Tests
         File:  dstrings-test.fs
       Author:  David N. Williams
      Version:  0.7.6
      License:  public domain
     Log file:  dstrings.log
Starting date:  August 31, 2008
Last revision:  September 4, 2008

Version 0.7.5
Starting date:  July 29, 2008
Last revision:  August 22, 2008

Version 0.7.4
Starting date:  July 29, 2008
Last revision:  August 14, 2008

Version 0.7.3
Starting date:  October 8, 2006
Last revision:  October 8, 2006

Version 0.7.2
Starting date:  May 14, 2004
Last revision:  July 19, 2004

Version 0.9.0   [version number above bumped back]
Starting date:  June 7, 2003
Last revision:  June 12, 2003

When used with dstrings.fs, this code is compatible with ANS
Forth, with an environmental dependence on lower case.  It can
also be used to test the pfe dstrings module.

This program calls dstester.fs, for which the following words
need sufficient prior testing:

  ($:  $DEPTH  $@  $!  $S>

The code here often uses under-the-hood constructions not
recommended for normal code, like manipulating msa's [measured
string addresses] on the data stack, and looking at bla's [back
link addresses].  It also tests a few words which are not in the
Dynamic-Strings word set, but which are in both the pfe and ANS
Forth implementations with the same [or later] version number as
this file.  Other under-the-hood words are tested only
implicitly.  The tests aim to be a good workout, but of course
do not prove the absence of bugs.

We use { ... -> ... } instead of ${{ ... => ... }}$ when we
don't want a test to empty the string stack, and we're not
testing string stack results.

Note that => records both data and string stacks, and }}$
compares both data and string contents [not string addresses].

The independent program trydstrings.fs can also be used for
trouble-shooting.
)

\ Change this to false to test with pfe DSTRINGS-EXT intead of
\ dstrings.fs:
\
true [IF]  \ USER-CONFIG
  s" dstrings.fs" included  \ ANS Forth version
  cr .( Testing dstrings.fs.)
  : .tested  ( -- )  ." dstrings.fs" ;
[ELSE]
  s" DSTRINGS-EXT" environment?
  0= [IF] cr .( ***pfe DSTRINGS-EXT environment not available)
          ABORT [THEN]
  ( version) drop
  cr .( Testing pfe DSTRINGS-EXT environment.)
  : .tested  ( -- )  ." pfe DSTRINGS-EXT environment" ;
[THEN]

112 constant /test-$buffer
  4 constant max-#test-$frames

s" dstester.fs" included
true verbose !
decimal

s" [UNDEFINED]" pad c! pad char+ pad c@ move 
pad find nip 0=
[IF]
: [UNDEFINED]  ( "name" -- flag )
  bl word find nip 0= ; immediate
[THEN]

[UNDEFINED] \\ [IF]
  : \\   BEGIN -1 parse 2drop refill 0= UNTIL ; [THEN]
[UNDEFINED] cell- [IF]
  : cell-  ( addr -- addr-cell )  [ 1 cells ] literal - ; [THEN]
[UNDEFINED] 2cells [IF]
  2 cells constant 2cells [THEN]


\ *** TROUBLE-SHOOTING WORDS

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
  cr cat$p@ IF ." Concatenation is in progress at: 0x"
      cat$p@ .
    ELSE ." No concatenation is in progress." THEN
  base ! ;

: dump-strings  ( -- )
  dstrings @ ( addr)
  /$buf /$space-header + /$frame-stack + cell+ ( #bytes)
  dump .$stat ;

: dump-mstring  ( a$ -- )
  dup mcount@ /MCOUNT + aligned dump ;

: .stacks  .s ." $depth: " $depth . ;


\ *** TESTS

cr
TESTING  /MCOUNT (M!) PARSE>S S` M,S

s" This is a measured string with 45 characters."
    m,s 2constant m1-s

cr .( Measured string count-field size: /MCOUNT =  ) /MCOUNT .
cr
cr .( You should see a string duplicated with the same)
cr .( alignment and between the same quotes:)
cr .( "This is a measured string with 45 characters.")
cr .( ") m1-s type .( ")
cr

cr .( Check that the measured string dump has a 0x2D count and the)
cr .( right count-field size, and is zero filled to alignment:)
m1-s -mcount dump-mstring
cr .( The address just after the zero padding should be: )
   m1-s + aligned hex . decimal
cr

cr .( The following dump should look the same as the above, except)
cr .( for its address and possible trailing "x"'s:)
pad 64 char x fill
{ m1-s pad (m!)  ->  }
pad dump-mstring
cr

s` This is a measured string with 45 characters.`
    m,s 2constant m2-s  \ M,S because could be transient

: m3-s  ( -- s  )
  s` This is a measured string with 45 characters.` ;

: s^  ( "ccc<^>" char -- )  [char] ^ parse>s ; immediate

char ^ parse>s This is a measured string with 45 characters.^
    m,s 2constant m4-s

: m5-s  ( -- s )
  s^ This is a measured string with 45 characters.^ ;

{ m2-s m1-s compare -> false }
{ m3-s m1-s compare -> false }
{ m4-s m1-s compare -> false }
{ m5-s m1-s compare -> false }

cr
TESTING  DSTRINGS MAKE-$SPACE /$FRAME-ITEM MAX-#$FRAMES /$FRAME-STACK
(
Replace the default string space with a smaller one for testing.
)
  dstrings @ free ( ior) drop
  /test-$buffer max-#test-$frames make-$space dstrings !


${{ max-#$frames  =>  max-#test-$frames }}$
{ /$frame-stack  ->  max-#$frames /$frame-item * } 

(
This test of 0STRINGS is with an empty string space.  We'll do
another later.
)
  0strings
  cr .( Empty string space used for testing:)
  dump-strings

cr cr
TESTING  MCOUNT -MCOUNT $. $TYPE >$S $S> $POP $PUSH-EXT

${{ m1-s >$s  =>  m1-s -mcount $push-ext }}$
{ m1-s -mcount mcount  ->  m1-s }
${{ m1-s >$s $s>  =>  m1-s }}$
${{ m1-s >$s $pop =>  m1-s -mcount }}$
${{ m1-s -mcount $push-ext  =>  m1-s >$s }}$
${{ m1-s >$s $pop mcount  =>  m1-s }}$

: m$  ($: -- a$ )  m1-s >$s ;

cr .( You should see the same string on the next two lines without quotes:)
cr ${{ m$ $.  =>  }}$
cr ${{ m$ $type  => }}$
cr

(
Uncomment one line to test $stack underflow exceptions. 
)
\ $pop
\ $s>

cr
TESTING  $" $` $CONSTANT $VARIABLE MCOUNT@ MCOUNT! PARSE>$

cr .( You should see a string three times with the same)
cr .( alignment and between the same quotes:)
cr .( "This is a short string.")
cr .( ") ${{ $" This is a short string." $.  =>  }}$ .( ")

: ."short"  ( -- ) $" This is a short string." $. ;

cr .( ") ${{ ."short"  =>  }}$ .( ")
cr

$" Another short string." $constant ashort$1

: ashort$2  ($: -- a$ )  $" Another short string." ;

$` Another short string.` $constant ashort$3
: ashort$4  ($: -- a$ )  $` Another short string.` ;

char ^ parse>$ Another short string.^ $constant ashort$5

: $^  ( "ccc<^>" -- $: a$ )  [char] ^ parse>$ ; immediate
: ashort$6  ($: -- a$)  $^ Another short string.^ ;

${{ ashort$1  => $" Another short string." }}$
${{ ashort$1  => ashort$2 }}$
${{ ashort$1  => ashort$3 }}$
${{ ashort$1  => ashort$4 }}$
${{ ashort$1  => ashort$5 }}$
${{ ashort$1  => ashort$6 }}$

${{ : my$con  $constant ;  =>  }}$
${{ ashort$1 my$con my-ashort$  => }}$
${{ ashort$1  =>  my-ashort$ }}$

${{ $variable var$  =>  }}$
${{ var$ @ mcount@  =>  0 }}$  \ empty$

ashort$1 $pop var$ !

\ The following line changes an external string, often a no-no:
${{ 4 var$ @ mcount! var$ @ $push-ext  =>  $" Anot" }}$

${{ : my$var  $variable ;  =>  }}$
${{ my$var avar$  avar$ @ $push-ext  =>  empty$ }}$

cr
TESTING  \N$ EMPTY$

${{ empty$ $pop mcount@  =>  0 }}$
${{ \n$ $s> swap c@  =>  1 10 }}$

TESTING  $@ $! >$S >$S-COPY $S> $,S $S@ IN-$BUFFER?

\ Note that $! is already used in dstester.fs, but up to now
\ only with external strings.

${{ empty$ var$ $!  =>  }}$
${{ var$ $@  =>  empty$ }}$
ashort$1 var$ $!
${{ var$ $@  =>  ashort$1 }}$

s" short" m,s 2constant short-s
$" short" $constant short$ 

${{ short-s >$s  =>  short-s -mcount $push-ext }}$
${{ short-s >$s-copy var$ $!  =>  }}$
{ var$ @ in-$buffer?  ->  true }
{ short-s -mcount in-$buffer?  ->  false }

\ The following should produce a second copy of "short" in the
\ string buffer, because => does a $!, and var$ already has a
\ copy in the string buffer:
${{ var$ $@ => short$ }}$

${{ short$ $s>  =>  short$ $pop mcount }}$
${{ short$ $s@  =>  short$ $pop mcount short$ }}$
$" hello" $constant hello$
${{ here hello$ $,s 2constant hello-s  => hello-s -mcount }}$

cr .( You should see a correct dump of "hello" as a measured string,)
cr .( zero-filled to alignment:)
hello-s -mcount dump-mstring
cr

cr
TESTING  0STRINGS 0$SPACE

\ Test 0STRINGS with garbage and a string bound to VAR$:

s" garbage" >$s-copy $drop
s" bound"   >$s-copy var$ $!
{ var$ @ in-$buffer? $garbage? ->  true true }
${{ short$ 0strings $sp@ $bufp@ var$ $@  =>  $sp0@ $breakp@ empty$ }}$
${{ $unused $depth $frame-depth  =>  /test-$buffer 0 0 }}$

\ Test 0$SPACE with garbage and a string bound to VAR$:

0strings
variable scratch
s" garbage" >$s-copy $drop
s" bound"   >$s-copy var$ $!
short$                          \ another string on the $stack
var$ @ scratch !
{ var$ @ in-$buffer? $garbage? -> true true }
{ dstrings @ 0$space $sp@ $bufp@ $depth  ->  $sp0@ $breakp@ 0 }
{ var$ @  ->  scratch @ }   \ 0$SPACE leaves VAR$ dangling
empty$ $pop var$ !          \ undangle it
${{ $unused $frame-depth  =>  /test-$buffer 0 }}$

TESTING  $DROP $2DROP $DUP $2DUP $SWAP $NIP $OVER $TUCK $PICK $EXCHANGE

hello$ $constant h$   short$ $constant s$

(
Uncomment any single line to test $stack underflow.
)
\ $drop
\ $2drop
\ h$ $2drop
\ $dup
\ $2dup
\ h$ $2dup
\ $swap
\ h$ $swap
\ $nip
\ h$ $nip
\ $over
\ h$ $over
\ $tuck
\ 0 $pick
\ h$ 1 $pick
\ 0 0 $exchange
\ h$ 1 0 $exchange
\ h$ $dup $dup 0 3 $exchange
\ h$ $dup $dup 3 3 $exchange

${{ h$       $drop   =>  }}$
${{ h$ h$    $drop   => h$ }}$
${{ h$ h$    $2drop  => }}$
${{ h$ h$ h$ $2drop  =>  h$ }}$

${{ h$ $dup      =>  h$ h$ }}$
${{ h$ s$ $2dup  =>  h$ s$ h$ s$ }}$

${{ h$ s$ $swap  =>  s$ h$ }}$
${{ h$ s$ $nip   =>  s$ }}$
${{ h$ s$ $over  =>  h$ s$ h$ }}$
${{ h$ s$ $tuck  =>  s$ h$ s$ }}$

${{ h$    0 $pick  =>  h$ h$ }}$
${{ h$ s$ 1 $pick  =>  h$ s$ h$ }}$


$" a" $constant a$
$" b" $constant b$

${{ h$ s$ 0 1 $exchange => s$ h$ }}$
${{ h$ s$ 1 0 $exchange => s$ h$ }}$
${{ h$ a$ s$ s$ 1 3 $exchange => s$ a$ h$ s$ }}$
${{ h$ a$ b$ s$ a$ 4 1 $exchange => s$ a$ b$ h$ a$ }}$

TESTING  bound string back-link and auto-copy rule
(
The auto-copy rule is that the only time a bound string is
copied without explicit demand is by $! when it stores a string
already bound to a string variable into another string variable.
)

0strings
\ These two lines should leave a copy of "hello" bound to VAR$
\ and a copy of "short" bound to the string stack:

  h$ $s> >$s-copy var$ $!
  var$ $@ s$ $s> >$s-copy  ($: h$ s$)

\ binding tests

: blink@  ( msa -- blink )  cell- @ ;
{ var$ @ in-$buffer?         ->  true }
{ var$ @ blink@              ->  var$ }
{ $sp@ @ blink@              ->  $sp@ }
{ var$ $@ $s@ -mcount blink@ ->  var$ }   ($: h$ s$ h$)
{ s$ var$ $! $depth          ->  3 }      ($: h$ s$ h$)
{ $sp@ @ blink@              ->  $sp@ 2cells + }
{ $2drop $sp@ @ blink@       ->  $sp@ }   ($: h$)
{ $garbage?                  ->  true }
{ $sp@ cell- @ blink@        ->  0 }      \ s$ copy is garbage
{ var$ $@ $sp@ @ in-$buffer? ->  false }  ($: h$ s$-ext)
$drop                                     ($: h$)

\ copying tests

\ bound h$ not copied when stored on top of external s$ 
${{ $sp@ @ var$ $!          =>  var$ @ }}$
\ nor when stored on top of itself
${{ var$ @ var$ $@ var$ $!  =>  var$ @ }}$

$variable var2$   var$ $@ var2$ $!   \ should make a new copy of h$
{ var$ @  in-$buffer?  ->  true }    \ old copy in string buffer
{ var2$ @ in-$buffer?  ->  true }    \ new copy in string buffer  
{ var$ @  var2$ @ <>   ->  true }    \ new copy distinct from old
${{ var$ $@  =>  var2$ $@ }}$        \ but really is a copy
\ note that => may have generated one or more copies as well

TESTING  $SWAP and $EXCHANGE relinking

\ We don't claim to have exercised all paths in the code.

0strings

a$ $s> >$s-copy var$ $!   b$ $s> >$s-copy var2$ $!

\ strings bound to string stack
: a-bnd$  ($: -- a$ )  s" a" >$s-copy ;
: b-bnd$  ($: -- b$ )  s" b" >$s-copy ;

: th-blink  ( i $: a_i$ ... -- a_i$ ... s: blink_i )
  $pick $s> -mcount blink@ ;

${{ var$ $@ s$ $swap var$ @ blink@ => s$ var$ $@ var$ @ blink@ }}$
${{ s$ var$ $@ $swap var$ @ blink@ => var$ $@ s$ var$ @ blink@ }}$

${{ a$ b-bnd$ 0 th-blink $swap 1 th-blink
    => b$ a$ $sp@ dup cell+ }}$

${{ a-bnd$ b-bnd$ 0 th-blink 1 th-blink $swap
    0 th-blink 1 th-blink
      => b$ a$ $sp@ dup cell+ 2dup }}$

${{ a-bnd$ b-bnd$ $over 1 th-blink 0 th-blink $swap
    0 th-blink 1 th-blink
      => a$ a$ b$ $sp@ cell+ dup cell+ $sp@ dup 2 cells + }}$

${{ a$ b-bnd$ 0 th-blink 0 1 $exchange 1 th-blink
      => b$ a$ $sp@ dup cell+ }}$

${{ a-bnd$ b-bnd$ 0 th-blink 1 th-blink 1 0 $exchange
    0 th-blink 1 th-blink
      => b$ a$ $sp@ dup cell+ 2dup }}$

${{ b-bnd$ b-bnd$ a$ 1 th-blink 2 th-blink 1 2 $exchange
    1 th-blink 2 th-blink
      => b$ b$ a$ $sp@ cell+ dup cell+ 2dup }}$

${{ a-bnd$ h$ s$ b-bnd$ h$ 1 th-blink 4 th-blink 1 4 $exchange
  1 th-blink 4 th-blink
    => b$ h$ s$ a$ h$ $sp@ cell+ dup 3 cells + 2dup }}$

TESTING  $+ S+ PARSE-S+ $+" $+` ENDCAT

0strings
: bl+          ( -- )  $+"  " ;
: $+`this_is_`  ( -- )  $+` this is ` ;
: $+^  ( -- )    ( -- )  [char] ^ parse-s+ ; immediate
: $+^this_is_^  ( -- )  $+^ this is ^ ;
: cat?  ( -- flag )  cat$p@ 0= invert ;

$" short hello"   $constant s_h$
$" this is short" $constant d$

${{ s$ $+ bl+ h$ $+    endcat =>  s_h$ }}$ 
${{ $+`this_is_` s$ $+ endcat =>  d$ }}$
${{ $+^this_is_^ s$ $+ endcat =>  d$ }}$

${{ s$ $+ $+"  " $+" hello" endcat => s_h$ }}$
${{ $+" short" bl+ h$ $+ endcat  => s_h$ }}$
${{ s$ $+ $+`  ` $+` hello` endcat => s_h$ }}$
${{ $+` short` bl+ h$ $+ endcat  => s_h$ }}$

{ cat? empty$ $+ cat? -> 0 0 }
{ empty$ $+ short-s >$s-copy $drop -> }
{ cat? s" " s+ cat? -> 0 0 }
{ cat? $+" " cat? -> 0 0 }
{ cat? $+` ` cat? -> 0 0 }
{ cat? :noname $+` `; execute cat? -> 0 0 }

0strings
${{ s" this is " s+ s" short" s+ endcat  =>  d$ }}$
${{ char ^ dup parse-s+ this is ^ parse-s+ short^ endcat  =>  d$ }}$
${{ s$ $+ => }}$
${{ bl+ h$ $+  => }}$
${{ endcat  =>  s_h$ }}$
${{ endcat  =>  empty$ }}$
{ cat? char ^ parse-s+ ^ cat? -> 0 0 }
{ cat? :noname [char] ^ parse-s+ ; execute ^ cat? -> 0 0 }

0strings
${{ short-s >$s-copy $dup $+ bl+ h$ $+ ENDCAT => s$ s_h$ }}$

(
Uncomment the following line to test the concatenation lock.
)
\ h$ $+ s$ >$s-copy

\ Test that ENDCAT turns off the cat lock and that the cat$
\ gets marked as garbage by $DROP (probably superfluous):

0strings
{ h$ $+ cat$p@ endcat cat$p@ $drop swap blink@  ->  0 0 }

cr
TESTING  $FRAME DROP-$FRAME FIND-$ARG $FRAME-DEPTH

0strings
(
Uncomment one of the following lines to test a $frame exception.
)
\ 0 $frame 0 $frame 0 $frame 0 $frame 0 $frame \ $frame stack overflow
\ drop-$frame              \ $frame stack underflow
\ 1 $frame                 \ not enough strings ...
\ 0 $frame h$ 2 $frame     \ ... for frame

${{ $frame-depth  =>  0 }}$
${{ 0 $frame  $frame-depth  ->  1 }}$
{ h$ s$ s_h$ 3 $frame $frame-depth ->  2 }
{ h$ $s> find-$arg  ->  2 true }
{ s$ $s> find-$arg  ->  1 true }
{ s_h$ $s> find-$arg  ->  0 true }

${{ d$ $s> find-$arg drop-$frame $frame-depth  =>  false 1 }}$

\ Repeat with the string frame below the top string:
{ h$ s$ s_h$ 3 $frame $frame-depth d$  ->  2 }
{ h$ $s> find-$arg  ->  2 true }
{ s$ $s> find-$arg  ->  1 true }
{ s_h$ $s> find-$arg  ->  0 true }

{ $s> find-$arg drop-$frame $frame-depth $depth  ->  false 1 0 }
{ drop-$frame $frame-depth  ->  0 }  \ dropped frame had zero items

0strings
{ h$ s$ s_h$ 3 $frame $frame-depth  ->  1 }
0strings
{ h$ s$ s_h$ 3 $frame d$ 1 $frame $frame-depth  ->  2 }
0strings
{ $frame-depth $fsp@ $sp@ $breakp@  ->  0 $fsp0@ $sp0@ $bufp@ }

\ Drop string frame below other strings:
0strings
${{ 0 $frame a$ drop-$frame $frame-depth => a$ 0 }}$
${{ a$ b$ 2 $frame s$ drop-$frame $frame-depth => s$ 0 }}$
${{ a-bnd$ b-bnd$ 2 $frame s$ drop-$frame $frame-depth => s$ 0 }}$
${{ a$ 1 $frame b$ s$ drop-$frame $frame-depth => b$ s$ 0 }}$
${{ a-bnd$ 1 $frame b-bnd$ s$ drop-$frame $frame-depth => b$ s$ 0 }}$
${{ a$ b$ 2 $frame s$ 1 $frame h$ drop-$frame $frame-depth
      => a$ b$ h$ 1 }}$
0strings
${{ h$ s$ 2 $frame a-bnd$ b-bnd$ 2 $frame h$ drop-$frame
    collect-$garbage $frame-depth
      => h$ s$ h$ true 1 }}$

cr
TESTING  COLLECT-$GARBAGE $GC-OFF $GC-ON $GC-LOCK@ $GC-LOCK!

0strings
s" goodbye" m,s 2constant goodbye-s
s" junk"    m,s 2constant junk-s

: $space-clear?  ( -- flag )
  $frame-depth 0=   $fsp@ $fsp0@ = and   $sp@ $sp0@ = and
      $breakp@ $bufp@ = and   cat$p@ 0= and ;

(
Make a garbage hole at the beginning of the string buffer, a
hole between bound strings, and a hole as the last string in the
buffer, with some duplication on the string stack, and something
stored in a string variable.
)
  short$
  junk-s    >$s-copy $drop         \ beginning hole
  hello-s   >$s-copy $dup
  junk-s    >$s-copy $drop         \ double middle hole
  junk-s    >$s-copy $drop
  goodbye-s >$s-copy $dup var$ $!
  junk-s    >$s-copy $drop         \ end hole

{ $garbage? ->  true } 

cr .( You should see a garbage hole at the start of the string buffer,)
cr .( a string bound to and duplicated on the string stack, a garbage)
cr .( hole with two strings, a string bound to a variable and)
cr .( duplicated on the stack, and a garbage hole with one string.)
cr .( The fourth, deepest string on the stack is external.)
dump-strings

s" This is a new string." >$s-copy

cr
cr .( Now we have triggered a garbage collection by copying in)
cr .( another string, so you should see the holes removed.)
dump-strings

{ collect-$garbage  ->  false }
${{ $2drop $2drop $drop $depth  =>  0 }}$


\ Now it's all garbage except for the next to last string, which
\ is stored in var$.
${{ $garbage? collect-$garbage $garbage?  =>  true true false }}$

\ Only the var$ is left.
{ var$ @ in-$buffer?  ->  true }
${{ empty$ var$ $! $garbage? => true }}$

\ Now it's garbage, too.
${{ collect-$garbage $garbage?  =>  true false }}$
${{ $space-clear?  =>  true }}$

(
Next we test COLLECT-$GARBAGE with the first string alive,
followed by several garbage strings.
)
 0strings
 hello-s   >$s-copy
 junk-s    >$s-copy $drop
 goodbye-s >$s-copy $drop
 junk-s    >$s-copy $drop

${{ collect-$garbage $drop collect-$garbage => true true }}$
{ $space-clear? -> true }

(
Test with nothing but garbage.
)
 0strings
 hello-s   >$s-copy $drop
 junk-s    >$s-copy $drop
 goodbye-s >$s-copy $drop

{ collect-$garbage $space-clear?  ->  true true }

\ GC LOCK TEST

\ Make some garbage.
${{ hello-s >$s-copy $drop $gc-off  =>  }}$
{ $gc-lock@ -> true }

(
Uncomment the following line to test the garbage collection
exception on attempting gc when it's turned off.
)
\ collect-$garbage

${{ $gc-on collect-$garbage  =>  true }}$
{ $gc-lock@ -> false }

{ true  $gc-lock! $gc-lock@ -> true }
{ false $gc-lock! $gc-lock@ -> false $gc-on }

cr cr
TESTING  #$ARGS $ARGS{ TH-$ARG
(
Uncomment any one of the following to see an exception:
)
\ $ARGS{      \ interpreting compile-only
\ #$args      \ $frame-stack underflow
\ 0 th-$arg   \ $frame-stack underflow

{ h$ s$ s_h$ 3 $frame #$args  ->  3 }
(
Uncomment the following line to see an exception:
)
\ 3 th-$arg   \ not enough strings in top frame
{ s_h$ 0 th-$arg $s> $s> compare  ->  0 }
{ h$ 2 th-$arg $s> $s> compare  ->  0 } 
{ s$ #$args $frame-depth $depth  ->  3 1 4 }

\ Example from documentation:
: george  ($: a$ b$ c$ -- cat$ )
      $ARGS{ arg1 arg2 arg3 }
      $+" This is arg1:  " arg1 $+" ." ENDCAT ;

0strings
${{ $" bill"  $" sue"  $" marie"  george =>
    $" This is arg1:  bill." }}$

: 3args  ($: a$ b$ c$ -- )
  $+" Arguments: " $ARGS{ 
                           alpha beta
                           gamma
                         }
  $+` one = "` alpha $+` ", two = "` beta
  $+` " three = "` gamma $+` ".` ENDCAT var$ $!
  alpha beta gamma ENDCAT avar$ $! ;

0strings
$" What " $" a " $" blast!" 3args
${{ $` Arguments: one = "What ", two = "a " three = "blast!".`
    => var$ $@ }}$

cr avar$ $@ $.
cr .( Done testing ) .tested .(  with /MCOUNT = )
    /MCOUNT 1 u.r .( .)
cr .( #ERRORS: ) #errors @ .
cr
