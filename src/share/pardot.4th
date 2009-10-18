\
\ pardot.4th ---	Paren-dot words, i.e. formatting numbers in strings
\
\ (duz 26Jul94)
\

: (UD.)		( ud --- )	<# #S #> ;
: (U.)		( u --- )	0 (UD.) ;

: (D.)		( d --- )	TUCK DABS <# #S ROT SIGN #> ;
: (.)		( n --- )	DUP ABS 0 <# #S ROT SIGN #> ;
