\
\ sh_libs.4th ---	Library code for pfe, Linux' shared libraries.
\			By Kevin Haddock.
\
\ (duz 22May94)
\

\ =======================================================================
\ Shared libraries for Linux
\ =======================================================================

S" HOST-SYSTEM" ENVIRONMENT? DROP S" Linux" COMPARE 0= [IF]

\ creates shared library variable
: GOT ( N _)   CREATE ,  DOES> ( a)  @ @ ;

\ creates shared library call
: PLT ( N _)   CREATE ,  DOES> ( ... cw a - res)  @ CALL-C ;

[THEN]

\ =======================================================================
\ end of sh-libs.4th
\ =======================================================================

CR .( Linux shared library calls loaded. )
