\
\ library.4th ---	Library code for pfe, load file.
\
\ (duz 22May94)
\

REDEFINED-MSG OFF
: HAVE		BL WORD FIND NIP ;
: [IFDEF]	HAVE    POSTPONE [IF] ; IMMEDIATE
: [IFNDEF]	HAVE 0= POSTPONE [IF] ; IMMEDIATE
REDEFINED-MSG ON

[IFDEF] FORGET-LIBRARY FORGET-LIBRARY [THEN]
ONLY  EXTENSIONS ALSO  FORTH ALSO DEFINITIONS  DEFAULT-ORDER
MARKER FORGET-LIBRARY

\ =======================================================================
\ include individual features
\ =======================================================================

  INCLUDE sh-libs
  INCLUDE loading
  INCLUDE defer-is
  INCLUDE doermake
  INCLUDE arrays
  INCLUDE pardot

\ =======================================================================
\ String stuff
\ =======================================================================

: "		POSTPONE S" OVER + 0 SWAP C! ;
