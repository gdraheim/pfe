\
\ library.4th ---	Library code for pfe.
\
\ (duz 22May94)
\

\ =======================================================================
\ multi-dimensional array type
\ =======================================================================

\ Multi-dimensional arrays are supported by two primitives in the kernel
\ doing the multiplication work:
\	BUILD-ARRAY \ n1 n2 ... nX X --- n
\		takes a list of upper bounds plus the dimension of the array,
\		writes those upper bounds to the dictionary, returns their
\		product.
\	ACCESS-ARRAY \ i1 i2 ... iX addr1 --- addr2 offset
\		addr1 points to a list generated by BUILD-ARRAY.
\		Multiplies the actual indices i with the registered bounds
\		pointed to by addr1.
\		Returns the offset in items of the accessed element,
\		addr2 points to just after the list of upper bounds.
\		Indices start with 0, highest index iY is nY - 1.
\		On index out of range ACCESS-ARRAY throws -2051.
\ Usage is simple and demonstrated below with an array of Cells:

: CELL-ARRAY	CREATE	\ n1 n2 ... nX X --- ; X is dimension of ARRAY
			BUILD-ARRAY		\ --- size in items
			CELLS			\ --- size in address units
			HERE OVER ALLOT		\ allocate space
			SWAP ERASE		\ and wipe it
		DOES>	\ i1 i2 ... iX --- addr
			ACCESS-ARRAY		\ addr item-offset
			CELLS + ;

\ =======================================================================
\ end of arrays.4th
\ =======================================================================

CR .( Array types loaded. )
