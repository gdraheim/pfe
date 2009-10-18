\
\ loading.4th ---	Library code for pfe, utilities for loading blocks.
\
\ (duz 22May94)
\

\ =======================================================================
\ more on blocks and loading
\ =======================================================================

: #BLOCKS	\ --- u ; number of blocks in current block file
		BLOCK-FILE FILE-SIZE ?FILE  1024 UM/MOD NIP ;

: +LOAD		\ n --- ; load screen relative
		?LOADING BLK @ + LOAD ;

: +THRU		\ first last --- ; like THRU, relative block numbers
		?LOADING SWAP BLK @ + SWAP BLK @ + THRU ;

: INDEX		\ first last --- ; show first lines of block range
		START?CR
		1+ SWAP DO  ?CR I 3 .R SPACE  I BLOCK 64 TYPE
		LOOP SPACE ;

: QX		\ --- ; show first lines partially of every block in file
		CR START?CR
		#BLOCKS 0 DO
		    OUT @ 20 + XMAX >= IF ?CR THEN
		    I 3 .R SPACE  I BLOCK 16 TYPE
		LOOP SPACE ;

\ =======================================================================
\ end of loading.4th
\ =======================================================================

CR .( Loading utilities loaded. )
