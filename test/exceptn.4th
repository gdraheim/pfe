\
\ test/exception.4th --- tests the exception word set
\ (duz 10Aug93)
\

[DEFINED] D [NOT] [IF] : D DEPTH . ; [THEN]
[DEFINED] Y [NOT] [IF] : Y IF ." /Y/ " ELSE ." BAD/Y/ " THEN ; [THEN]
[DEFINED] N [NOT] [IF] : N IF ." BAD/N/ " ELSE ." /N/ " THEN ; [THEN]

CR .( exception words)

0 [IF] \ old style code: floating not always compiled, and on some
       \ platforms float-div-by-zero returns simply infinity.
CR .( -- ) CR
: FCL		FDEPTH 0 > IF  FDEPTH 0 DO  FDROP  LOOP  THEN ;
D SPACE					\ stack must be empty on entry!

CR .( system throws: - stack underflow / floating point exception [FPE] ) CR
: STK		DROP ?STACK ;		\ a stack underflow exception
: FPE		1E0 0E0 F/ ;		\ a floating point exception
 
: EX1		['] STK CATCH ;		EX1  DUP . -4 = Y  D SPACE
: EX2		['] FPE CATCH ;		EX2  DUP . -55 = Y  D SPACE

CR .( FPE inside evaluate: - catched on commandline / inside a word ) CR
: FPEE		S" FPE" EVALUATE ;	' FPEE CATCH -55 = Y  D FCL SPACE
: FPEEE		S" ' FPEE CATCH" ;	FPEEE EVALUATE -55 = Y  D FCL SPACE
[ELSE]

CR .( system throws: - stack underflow / invalid memory access [sigbus] ) CR
: STK           DROP  ?STACK ;          \ a stack underflow condition
: PTR           0 @ ;                   \ invalid memory address
: TRW           3 THROW ;               \ argument throw

: -9_= DUP -9 = IF EXIT THEN -23 = ;    \ SIGSEGV (-9) or SIGBUS (-23)

: EX1		['] STK CATCH ;		EX1  DUP . -4 = Y  D SPACE
: EX2		['] PTR CATCH ;		EX2  DUP . -9_= Y  D SPACE
: EX3           ['] TRW CATCH ;         EX3  DUP .  3 = Y  D SPACE

CR .( throw inside evaluate: - catched on commandline / inside a word ) CR
: TRWE	S" TRW" EVALUATE ;	' TRWE CATCH   DUP .  3 = Y  D SPACE
: TRWEE	S" ' TRWE CATCH " ;     TRWEE EVALUATE DUP .  3 = Y  D SPACE

CR .( sigbus inside evaluate: - catched on commandline / inside a word ) CR
: PTRE	S" PTR" EVALUATE ;	' PTRE CATCH   DUP . -9_= Y  D SPACE
: PTREE	S" ' PTRE CATCH " ;     PTREE EVALUATE DUP . -9_= Y  D SPACE

[THEN]

CR .( evaluate catch ) CR
: UNDEF		S" :-) 0 Y " ;		UNDEF ' EVALUATE CATCH -13 = Y
					2DROP  D

\
\ Test handling signals with Forth words.
\

CR
: SIG-TERM-HDL ." SIGTERM handled ok. " ;
' SIG-TERM-HDL SIGTERM FORTH-SIGNAL .
SIGTERM RAISE-SIGNAL
0 SIGTERM FORTH-SIGNAL ' SIG-TERM-HDL = Y



