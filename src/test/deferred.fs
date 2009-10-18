\ example implementation and test cases

: defer ( "name" -- )
  create ['] abort ,
does> ( ... -- ... )
  @ execute ;

: defer@ ( xt1 -- xt2 )
  >body @ ;

: defer! ( xt2 xt1 -- )
  >body ! ;

: is
  state @ if
    POSTPONE ['] POSTPONE defer!
  else
    ' defer!
  then ; immediate

: action-of
 state @ if
   POSTPONE ['] POSTPONE defer@
 else
   ' defer@
then ; immediate

\ test cases
require test/tester.fs

{ defer defer1 -> }
{ : is-defer1 is defer1 ; -> }
{ : action-defer1 action-of defer1 ; -> }
{ ' * ' defer1 defer! -> }
{ 2 3 defer1 -> 6 }
{ ' defer1 defer@ -> ' * }
{ action-of defer1 -> ' * }
{ action-defer1 -> ' * }
{ ' + is defer1 -> }
{ 1 2 defer1 -> 3 }
{ ' defer1 defer@ -> ' + }
{ action-of defer1 -> ' + }
{ action-defer1 -> ' + }
{ ' - is-defer1 -> }
{ 1 2 defer1 -> -1 }
{ ' defer1 defer@ -> ' - }
{ action-of defer1 -> ' - }
{ action-defer1 -> ' - }
