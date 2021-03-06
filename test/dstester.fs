(       Title:  Dynamic-Strings word set test support
         File:  dstester.fs
  Modified by:  David N. Williams
      License:  LGPL
      Version:  0.9.0
Starting date:  June 6, 2003
Last Revision:  September 9, 2004

For any code derived from John Hayes' tester program:
)
\ (C) 1995 JOHNS HOPKINS UNIVERSITY / APPLIED PHYSICS LABORATORY
\ MAY BE DISTRIBUTED FREELY AS LONG AS THIS COPYRIGHT NOTICE REMAINS.
(
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or at your option any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
MA 02111-1307 USA.

This code extends John Hayes' tester.fr to work with our
Dynamic-Strings word set, which has a separate string stack.

There is an ANS Forth environmental dependence on lower case.

Either dstrings.fs or the pfe dstrings module must be loaded to
run this program.

For this program to work, these words need sufficient prior
testing:  ($:  $DEPTH  $@  $!  $S>
)


\ Set the following flag to true for more verbose output; this
\ may allow you to tell which test caused your system to hang.

variable verbose
   false verbose !

: empty-stack  ( ... -- )
(
Empty the data stack.  Handles underflow, too.
)
  depth ?dup
  IF dup 0< 
    IF negate 0 DO 0 LOOP
    ELSE 0 DO drop LOOP
    THEN
  THEN ;

: empty-$stack  ( ... -- )
(
Empty the dstring stack.  Handles underflow, too.
)
  $depth ?dup
  IF dup 0< 
    IF negate 0 DO 0 LOOP
    ELSE 0 DO $drop LOOP
    THEN
  THEN ;

variable #errors   0 #errors !

: data-error  ( c-addr u -- )
(
Display an error message followed by the line that had the
error, and increment the error count.
)
  type cr source type cr	\ display line corresponding to error
  1 #errors +!
  empty-stack ;			\ throw away rest of data

: $error  ( c-addr u -- )
(
Display an error message followed by the line that had the
error, and increment the error count
)
  type cr source type cr	\ display line corresponding to error
  1 #errors +!
  empty-$stack ;	        \ throw away remaining strings


\ stack records

20 constant #stack-records
variable actual-depth
create actual-results #stack-records cells allot
variable actual-$depth
create actual-$results #stack-records cells allot
actual-$results #stack-records cells erase  \ initialize to external for gc

: empty-$results  ( -- )
(
Occasionally one may want to garbage any copies of bound
strings generated by doing $! into ACTUAL-$RESULTS.
)
  #stack-records 0 DO
    actual-$results i cells + empty$ $!
  LOOP ;

\ syntactic sugar
: {    ( -- )  ;
: ${   ( -- )  ;
: ${{  ( -- )  ;


: ->  ( ... -- )
(
Record the depth and content of data stack.
)
  depth dup actual-depth !		\ record depth
  ?dup IF				\ if there is something on stack
    0 DO				\ save them
      actual-results i cells + !
    LOOP
  THEN ;

: $->  ($: ... -- )
(
Record the depth and content of the string stack.  Using $! is
important in case garbage collection occurs.
)
  $depth dup actual-$depth !		\ record depth
  ?dup IF				\ if there is something on stack
     0 DO				\ save them
       actual-$results i cells + $!
     LOOP
  THEN ;

: =>  ($: ... s: ... -- ) -> $-> ;

: }  ( ... -- )
(
Compare the data stack [expected] contents with the saved
[actual] contents.
)
  depth actual-depth @ = IF		\ if depths match
    depth ?dup IF			\ if there is something on the stack
      0 DO				\ for each stack item
        actual-results
	    i cells + @			\ compare actual with expected
	<> IF s" incorrect data result: "
	   data-error LEAVE THEN
      LOOP
    THEN
  ELSE					\ depth mismatch
    s" wrong number of data results: " data-error
  THEN ;

: $<>  ($: a$ b$ -- s: flag )
(
Because storing into a string variable can cause a copy, and
because garbage collection can move strings, we do not check
string equality by just comparing the mfa's.
)
  $s> $s> compare ;

: }$  ($: ... -- )
(
Compare the string stack [expected] contents with the saved
[actual] contents.
)
  $depth actual-$depth @ = IF		\ if depths match
    $depth ?dup IF			\ if there is something on the stack
      0 DO				\ for each stack item
        actual-$results
	    i cells + $@		\ compare actual with expected
	$<> IF s" incorrect string result: "
	    $error LEAVE THEN
      LOOP
    THEN
  ELSE					\ depth mismatch
    s" wrong number of string results: " $error
  THEN ;

: }}$  ( -- )  } }$ ;

: TESTING  ( -- )  \ talking comment
  source verbose @
  IF dup >r type cr r> >in !
  ELSE >in ! drop
  THEN ;
