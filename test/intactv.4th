\
\ intactv.4th --- test some interactive features
\ (duz 13May94)
\

MARKER FORGET-INTERACTIVE-TESTS

\ Screen

CLS

S" This is the center of a blank screen. Key..."
COLS OVER - 2/ 1- ROWS 2/ 1- AT-XY TYPE KEY DROP

CLS		.( Testing text attributes on screen:         ) CR CR
.NORMAL		.( This is NORMAL text. ) CR
.HIGHLIGHT	.( This text is HIGHLIGHTed.                  )
.HIGHLIGHT.OFF	.(  This is again NORMAL text. ) CR
.UNDERLINE	.( This text is UNDERLINEd.                   )
.UNDERLINE.OFF	.(  This is again NORMAL text. ) CR
.INTENSITY	.( This text is displayed with more INTENSITY.)
.INTENSITY.OFF	.(  This is again NORMAL text. ) CR
.BLINKING	.( This is BLINKING text.                     )
.BLINKING.OFF	.(  This is again NORMAL text. ) CR
.REVERSE		.( This text is displayed in REVERSE video.   )
.REVERSE.OFF	.(  This is again NORMAL text. ) CR

D FD WAIT

\ Keyboard

CLS .( Now testing the keyboard. )

: EMPTY-INPUT	10 0 DO
		    20 MS
		    EKEY? IF  EKEY DROP  ELSE  LEAVE  THEN
		LOOP ;

: TEST-KEY	CR ." Press "
		SAVE-INPUT		BL PARSE TYPE SPACE
		RESTORE-INPUT DROP	EKEY ' EXECUTE =
		IF    ."  Thanks! "
		ELSE  ."  Not recognized." EMPTY-INPUT
		THEN ;

CR
CR .( Please press the requested keys to see if pfe detects them right.)
CR .( K-PRIOR and K-NEXT are page up/down on many keyboards.)
CR .( If your keyboard has no such key, just press any other and live)
CR .( with it.)
CR

TEST-KEY K-LEFT
TEST-KEY K-RIGHT
TEST-KEY K-UP
TEST-KEY K-DOWN
TEST-KEY K-HOME
TEST-KEY K-END
TEST-KEY K-PRIOR
TEST-KEY K-NEXT

TEST-KEY K1	TEST-KEY K2	TEST-KEY K3	TEST-KEY K4	TEST-KEY K5
TEST-KEY K6	TEST-KEY K7	TEST-KEY K8	TEST-KEY K9	TEST-KEY K10

D
CR
CR .( Disappointed?)
CR .( Many -- most -- systems have bad termcap files. You can get all keys)
CR .( to work if you edit your /etc/termcap file and insert the k0 through)
CR .( k9 and some other items. Consult your system's documentation.)
WAIT

\ Forth words on function keys K1 through K10:

CLS
CR
CR .( You can bind Forth definitions to the keys K0 through K10.)
CR .( Now testing this feature.)
CR

: PRINT-ME ." Yeah!" ;
K1 EXECUTES PRINT-ME
CR .( Please press K1 and then return ... ) CR
PAD 10 ACCEPT DROP
CR .( It's ok if he said `Yeah!' )
: SET-K2 K2 EXECUTES PRINT-ME ;
SET-K2
CR .( Now please press K2 and return ... ) CR
PAD 10 ACCEPT DROP
CR .( Did he say `Yeah!' again? )
K1 EXECUTES NOOP
K2 EXECUTES NOOP
D

\ end of intactv.4th

FORGET-INTERACTIVE-TESTS
