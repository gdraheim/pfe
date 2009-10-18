\
\ test/stack.4th ---	test stack and return stack operators
\ (duz 05Aug93)
\

[UNDEFINED] D [IF] : D DEPTH . ; [THEN]

CR .( stack operators )

CR .( single co )
11 CONSTANT N1
22 CONSTANT N2
33 CONSTANT N3

CR N1 N2	DROP	.	D CR .( 11 0 ) 
CR N1 N2	DUP	. . .	D CR .( 22 22 11 0 ) 
CR N1 N2	?DUP	. . .	D CR .( 22 22 11 0 ) 
CR N1 0		?DUP	. . 	D CR .( 0 11 0 ) 
CR N1 N2	SWAP	. .    	D CR .( 11 22 0 ) 
CR N1 N2	OVER	. . .	D CR .( 11 22 11 0 ) 
CR N1 N2 N3	ROT	. . .	D CR .( 11 33 22 0 ) 
CR N1 N2	NIP	.     	D CR .( 22 0 )
CR N1 N2	TUCK	. . . 	D CR .( 22 11 22 0 )
CR N1 N2 N3	-ROT	. . .	D CR .( 22 11 33 0 )

CR .( double co )
11. 2CONSTANT D1
22. 2CONSTANT D2
33. 2CONSTANT D3

CR D1 D2	2DROP	D.		D CR .( 11 0 )
CR D1 D2	2DUP	D. D. D.	D CR .( 22 22 11 0 )
CR D1 D2	2SWAP	D. D.		D CR .( 11 22 0 )
CR D1 D2	2OVER	D. D. D.	D CR .( 11 22 11 0 )
CR D1 D2 D3	2ROT	D. D. D.	D CR .( 11 33 22 0 )

CR 

