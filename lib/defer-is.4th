\
\ library.4th ---	Library code for pfe.
\
\ (duz 22May94)
\

\ =======================================================================
\ F83-like DEFER and IS for vectorized execution
\ =======================================================================

: CRASH		\ --- ; default action when a DEFER is created
		TRUE ABORT" uninitialized DEFER called" ;

: DEFER		\ "word" --- \ yes you can do CONSTANT ... DOES> in pfe:
		['] CRASH CONSTANT DOES> PERFORM ;

: IS		\ xt --- \ store in PFA of following word
			 \ which should be defined by DEFER
		POSTPONE TO ; IMMEDIATE

\ =======================================================================
\ end of defer-is.4th
\ =======================================================================

CR .( DEFER...IS loaded. )
