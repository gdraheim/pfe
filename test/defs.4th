\
\ test/defs.4th ---	test VARIABLE and CONSTANT and simple colon definitions
\			provide some shortcuts
\ (duz 05Aug93)
\

CR .( simple colon definitions) CR

: T 0 . ; T	FORGET T
: T 00 . ; T	FORGET T
: T 0.0 D. ; T	FORGET T	SPACE

\ Y prints 0 if a clean true-flag is on top of stack
\ N prints 0 if a clean false-flag is on top of stack
\ D prints DEPTH

: Y	-1 = 0= . ;		-1 Y
: N	. ;			0 N
: D	DEPTH . ;		D
: FD	FDEPTH . ;		FD

: WAIT	CR ." press any key..." KEY DROP ;

CR .( constants) CR .(    )

DECIMAL

 123456789 CONSTANT N1		 123456789.123456789 2CONSTANT D1
 987654321 CONSTANT N2		 987654321.987654321 2CONSTANT D2
1111111110 CONSTANT N3		1111111111.111111110 2CONSTANT D3

N1 123456789 = Y		D1 123456789.123456789 D= Y
N2 987654321 = Y D		D2 987654321.987654321 D= Y D

: =N1 N1 = Y ;	: =N2 N2 = Y ;	: =N3 N3 = Y ;
: =D1 D1 D= Y ;	: =D2 D2 D= Y ;	: =D3 D3 D= Y ;

CR 
