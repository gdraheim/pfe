\
\ test/exception.4th --- tests the exception word set
\ (duz 10Aug93)
\

[DEFINED] D [NOT] [IF] : D DEPTH . ; [THEN]
[DEFINED] Y [NOT] [IF] : Y IF ." /Y/ " ELSE ." BAD/Y/ " THEN ; [THEN]
[DEFINED] N [NOT] [IF] : N IF ." BAD/N/ " ELSE ." /N/ " THEN ; [THEN]

CR .( exception words) CR .(    )

0 [IF] \ old style code: floating not always compiled, and on some
       \ platforms float-div-by-zero returns simply infinity.
: FCL		FDEPTH 0 > IF  FDEPTH 0 DO  FDROP  LOOP  THEN ;
D SPACE					\ stack must be empty on entry!

CR
: STK		DROP ?STACK ;		\ a stack underflow exception
: FPE		1E0 0E0 F/ ;		\ a floating point exception

: EX1		['] STK CATCH ;		EX1  DUP . -4 = Y  D SPACE
: EX2		['] FPE CATCH ;		EX2  DUP . -55 = Y  D SPACE

CR
: FPEE		S" FPE" EVALUATE ;	' FPEE CATCH -55 = Y  D FCL SPACE
: FPEEE		S" ' FPEE CATCH" ;	FPEEE EVALUATE -55 = Y  D FCL SPACE
[ELSE]
CR
: STK           DROP  ?STACK ;          \ a stack underflow condition
: PTR           0 @ ;                   \ invalid memory address

: EX1		['] STK CATCH ;		EX1  DUP . -4 = Y  D SPACE
: EX2		['] PTR CATCH ;		EX2  DUP . -9 = Y  D SPACE

CR
: PTRE		S" PTR" EVALUATE ;	' PTRE CATCH   DUP . -9 = Y  D SPACE
\ : PTREE	S" ' PTREE CATCH" ;	PTREE EVALUATE DUP . -9 = Y  D SPACE

[THEN]

CR
: UNDEF		S" :-) 0 Y " ;		UNDEF ' EVALUATE CATCH -13 = Y
					2DROP  D

\
\ Test handling signals with Forth words.
\

CR
: SIG-TERM-HDL	." SIGTERM handled ok. " ;
' SIG-TERM-HDL SIGTERM SIGNAL .
SIGTERM RAISE
0 SIGTERM SIGNAL ' SIG-TERM-HDL = Y



