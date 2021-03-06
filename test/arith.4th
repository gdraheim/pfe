\
\ arith.4th ---	test integer arithmetic operators
\ (duz 05Aug93)
\
CR

[DEFINED] D [NOT] [IF] : D DEPTH . ; [THEN]
[DEFINED] Y [NOT] [IF] : Y IF ." /Y/ " ELSE ." BAD/Y/ " THEN ; [THEN]
[DEFINED] N [NOT] [IF] : N IF ." BAD/N/ " ELSE ." /N/ " THEN ; [THEN]

CR .( integer add, subtract)

\ MARKER FORGET-ARITH-TEST

: T1	12 7 DO  I BASE !
		S"  123456543  123456543 - . " EVALUATE
		S" -123456543 -123456543 - . " EVALUATE
		S" -123456543  123456543 + . " EVALUATE
		S"  123456543 -123456543 + . " EVALUATE  SPACE
	LOOP  DECIMAL ;

CR .(    ) T1  SPACE D


: T2	12 7 DO  I BASE !
		S"  123456012.012345  123456012.012345 D- D. " EVALUATE
		S" -123456012.012345 -123456012.012345 D- D. " EVALUATE
		S" -123456012.012345  123456012.012345 D+ D. " EVALUATE
		S"  123456012.012345 -123456012.012345 D+ D. " EVALUATE  SPACE
	LOOP  DECIMAL ;

CR .(    ) T2  SPACE D


: T3	12 7 DO  I BASE !
		S" 2222222222 1111111111 + 3333333333 = Y " EVALUATE
		S" 3333333333 1111111111 - 2222222222 = Y " EVALUATE
	LOOP  DECIMAL ;

CR .(    ) T3  SPACE D


: T4	12 7 DO  I BASE !
		S" 222222222222222222. 111111111111111111. D+ "	EVALUATE
		S" 333333333333333333. D= Y "			EVALUATE
		S" 333333333333333333. 111111111111111111. D- "	EVALUATE
		S" 222222222222222222. D= Y "			EVALUATE
		SPACE
	LOOP  DECIMAL ;

CR .(    ) T4  SPACE D


CR .( integer multiply)

: T5	12 7 DO  I BASE !
		S"  123456  1000 *  123456000 = Y "	EVALUATE
		S"  1000  123456 *  123456000 = Y "	EVALUATE
		S" -123456 -1000 *  123456000 = Y "	EVALUATE
		S" -1000 -123456 *  123456000 = Y "	EVALUATE SPACE
	LOOP  CR ."    "
	12 7 DO  I BASE !
		S" -123456  1000 * -123456000 = Y "	EVALUATE
		S" -1000  123456 * -123456000 = Y "	EVALUATE
		S"  123456 -1000 * -123456000 = Y "	EVALUATE
		S"  1000 -123456 * -123456000 = Y "	EVALUATE SPACE
	LOOP  DECIMAL ;

CR .(    ) T5  SPACE D


: T6	12 7 DO  I BASE !
		S"  123456123     1000 M*  123456123.000    D= Y" EVALUATE
		S"  1000     123456123 M*  123456123.000    D= Y" EVALUATE
		S" -123456123 -1000000 M*  123456123.000000 D= Y" EVALUATE
		S" -1000000 -123456123 M*  123456123.000000 D= Y" EVALUATE
		SPACE
	LOOP  CR ."    "
	12 7 DO  I BASE !
		S" -123456123  1000000 M* -123456123.000000 D= Y" EVALUATE
		S" -1000000  123456123 M* -123456123.000000 D= Y" EVALUATE
		S"  123456123 -1000000 M* -123456123.000000 D= Y" EVALUATE
		S"  1000000 -123456123 M* -123456123.000000 D= Y" EVALUATE
		SPACE
	LOOP  DECIMAL ;

CR .(    ) T6  SPACE D


: T7	12 7 DO  I BASE !
		S" 123456123    10000 UM* 123456123.0000    D= Y" EVALUATE
		S" 10000 123456123    UM* 123456123.0000    D= Y" EVALUATE
		S" 65432165 100000000 UM* 65432165.00000000 D= Y" EVALUATE
		S" 100000000 65432165 UM* 65432165.00000000 D= Y" EVALUATE
		SPACE
	LOOP  DECIMAL ;

CR .(    ) T7  SPACE D


CR .( integer divide)

: T8	12 7 DO  I BASE !
		S" 123456543.2165 10000 UM/MOD "	EVALUATE
		S" 123456543 = Y "			EVALUATE
			S"   2165 = Y "			EVALUATE
		S" 123456.12345654 100000000 UM/MOD "	EVALUATE
		S" 123456 = Y "				EVALUATE
		      S"  12345654 = Y "		EVALUATE SPACE
	LOOP  DECIMAL ;

CR .(    ) T8  SPACE D


: T9	12 7 DO  I BASE !
		S" -123456543.2165 10000 SM/REM "	EVALUATE
		S" -123456543 = Y "			EVALUATE
			S"   -2165 = Y "		EVALUATE
		S" -123456.12345654 100000000 SM/REM "	EVALUATE
		S" -123456 = Y "			EVALUATE
		      S"  -12345654 = Y "		EVALUATE SPACE
	LOOP  DECIMAL ;

CR .(    ) T9  SPACE D


: T10	12 7 DO  I BASE !
		S"  123456543.2165 -10000 SM/REM "	EVALUATE
		S" -123456543 = Y "			EVALUATE
			S"    2165 = Y "		EVALUATE
		S"  123456.12345654 -100000000 SM/REM"	EVALUATE
		S" -123456 = Y "			EVALUATE
		      S"   12345654 = Y "		EVALUATE SPACE
	LOOP  DECIMAL ;

CR .(    ) T10 SPACE D


: T11	12 7 DO  I BASE !
		S" -123456543.2165 10000 SM/REM "	EVALUATE
		S" -123456543 = Y "			EVALUATE
			S"   -2165 = Y "		EVALUATE
		S" -123456.12345654 100000000 SM/REM "	EVALUATE
		S" -123456 = Y "			EVALUATE
		      S"  -12345654 = Y "		EVALUATE SPACE
	LOOP  DECIMAL ;

CR .(    ) T11 SPACE D


: T12	12 7 DO  I BASE !
		S" -123456543.2165 -10000 SM/REM "	EVALUATE
		S"  123456543 = Y "			EVALUATE
			S"  -2165 = Y "			EVALUATE
		S" -123456.12345654 -100000000 SM/REM "	EVALUATE
		S"  123456 = Y "			EVALUATE
		      S"  -12345654 = Y "		EVALUATE SPACE
	LOOP  DECIMAL ;

CR .(    ) T12 SPACE D

CR 
