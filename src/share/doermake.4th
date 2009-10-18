\
\ doermake.4th ---	Library code for pfe.
\
\ (duz 22May94)
\

ONLY FORTH ALSO DEFINITIONS

\ =======================================================================
\ Leo Brodie's DOER ... MAKE construct:
\ =======================================================================

: NOTHING ;

: DOER		CREATE	['] NOTHING >BODY ,
		DOES>	@ >R ;

: (MAKE)	R>  DUP CELL+
		DUP CELL+  SWAP @ >BODY !
		@ ?DUP IF  >R  THEN ;

: MAKE		STATE @ IF
		    POSTPONE (MAKE)  HERE 0 ,
		ELSE
		    HERE ' >BODY ! ]
		THEN
		; IMMEDIATE

: ;AND		POSTPONE EXIT  HERE SWAP !
		; IMMEDIATE

: UNDO		['] NOTHING >BODY  ' >BODY ! ;

\ let's test it:

DOER TEST
MAKE TEST	CR ." DOER...MAKE " ;
TEST
: T		MAKE TEST ." loaded. " ;AND  TEST ;
T
UNDO TEST
TEST
FORGET TEST

\ =======================================================================
\ end of doermake.4th
\ =======================================================================

RESET-ORDER
