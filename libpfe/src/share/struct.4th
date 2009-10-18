\
\	Structures and Bitmaps in Forth
\

\ Copyright (c) 1993 Missing Link All Rights Reserved
\ 975 East Ave, STE 112, California USA (916)343-8129
\ Copying permitted for non-resale in complete unaltered
\ form with the inclusion of this notice.
\ Software comes without warranty of any kind. Missing Link
\ disclaims all liability for damages of any kind resulting
\ from its use.


\ In order to understand the comments and datastructures in this tool
\ the following precise definitions should be of assistance.
\
\ Definitions:
\ Word		Description
\
\  Instance	An actual data structure created and allocated
\		in the dictionary.  Live data.  Consists of the
\		header, length cell, sizes of each dimention in the case
\		of arrays, and reserved data space.  The instance is
\		analogous with structures created by words created with
\		the typedef declarator in C.
\
\  Type		A defining word that creates and allocates instances of
\		a particular datastructure or creates element words of
\		compound datastructures (i.e. other more complex types).
\		The type can be optionally given one to three subscript
\		expressions each within square brackets to indicate the type
\		is to create a multi-dimensional array element or
\		instance.  (Note: The square brackets must be used in
\		interpreting mode else they will conflict with the Forth
\		versions which turn on and off the compiler.  See below
\		for examples)  The type is analogous to words created with
\		the typedef operator in C.
\
\  Element	A word that when given an address at run time will add
\		an internal offset to locate that sub-datastructure.  Can
\		be given one to three optional subscripts in the case the
\		element was defined as an array.  The element is analogous
\		of the 'dot operators' of a C datastructure.
\
\  Simple	A non-array instance or element.
\
\  Complex	An array instance or element.
\
\  Row		A slice of the array that consists of the first index
\		multiplied by the size of the type making up the array
\		added to the starting address of the array.
\
\ Plane		A two dimensional slice of a three dimensional array
\		analogous to the Row with an added dimension
\
\ Item		A single entry in an array.  Can consist of simple or
\		compound structures which can themselves be arrayed.
\
\ End definitions

decimal forth definitions 

c" structures" find nip [if]  execute  [then]  marker structures
c" defer" find nip 0= [if]  include library.4  [then]

\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
\ \								\
\ \	Data Record Format					\
\ \								\
\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

\ Name: Type Record
\
\ cell	Description
\ 0	whole size
\
\ Name: Simple Instance/Element
\
\ Cell	Description
\ 0	whole size
\ 1	data/offset
\
\
\ Name: Bitfield Element
\
\ Cell	Description
\ 0	whole size
\ 1	mask
\ 2	start bit location (1 byte long)
\ 3	offset
\
\ Name: One Dimensional Array Instance/Element
\
\ Cell	Description
\ 0	whole size
\ 1	item size
\ 2	data/offset
\
\ Name: Two Dimensional Array Instance/Element
\
\ Cell	Description
\ 0	whole size
\ 1	row size
\ 2	item size
\ 3	data/offset
\
\ Name: Three Dimensional Array Instance/Element
\
\ Cell	Description
\ 0	whole size
\ 1	plane size
\ 2	row size
\ 3	item size
\ 4	data/offset


\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
\ \								\
\ \	Required Variables and Values				\
\ \								\
\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

0 value defining

\ subscript array

3 1 cell-array subscript

\ number of bits requested in this element

0 value (bits)

\ current bitfield length

0 value bitfield-length

\ current bitfield next available bit

0 value next-bit


\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
\ \								\
\ \	Variable Support Words					\
\ \								\
\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

\ initialize all these values and variables

: /bitfield ( n - n')   next-bit if   bitfield-length +   then  0 to (bits)
   0 to bitfield-length   0 to next-bit ;

: /array   3 0 do   0 i subscript !   loop ;

: #subscripts ( - n)  3  3 0 do   i subscript @  0= if  drop i leave
   then  loop ;

\ the interpreted version of [ evaluates the code before the ] and
\ places the resultant value (the size of the array subscript) in the
\ proper place in the subscript array

: [ ( ___ ']')    state @ if  postpone [  else  [char] ] word
   count evaluate  #subscripts  subscript !   then ; immediate

: bit ( n)   to (bits) ;


\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
\ \								\
\ \	Bitfield Support Words					\
\ \								\
\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

\ Return bit value n3 from address a1 starting at bit n2 masked with mask n1

: b@ ( a1 n1 n2 - n3)  >r  swap @  and  r> rshift ;


\ Store bit value n1 to address a1 with mask n2 starting at bit position n3

: b! ( n1 a1 n2 n3)  >r  rot  r> lshift  over and  >r  invert  over @
   and  r> or   swap ! ;


\ given the number of bits required, return the number of bits
\ left over in the word.  Answer is negative if overflow.

: bits-left ( n1 - n2)   bitfield-length [ 1 cells ] literal min  8 *
   swap next-bit +  - ;


\ return the mask of the number of bits n1 and the start bit n2

: >mask ( n1 n2 - n')  2 swap ** swap ?dup if  1- 0 do  dup 1 lshift
   or  loop  then ; 


\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
\ \								\
\ \	Instanciation Support Words				\
\ \								\
\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\


\ return data address a3 given type record address (TRA) a2 and instance
\ address a1

\ : (simple) ( a1 a2 - a3)   nip ;	\ not needed but included for
					\ possible future expantion


\ given the index n, instance address a2 and TRA a1, return data address a3

: (1array) ( n a1 a2 - a3)  >r  [ 1 cells ] literal + @ *   r> + ;


\ given indices n1, n2, instance address a2, TRA a1, return data address a3

: (2array) ( n1 n2 a1 a2 - a3)   >r  dup >r  [ 2 cells ] literal +  @  *
   swap  r>  [ 1 cells ] literal +  @  *  + r>  + ;


\ given indices n1, n2, n3, instance address a2, TRA a1, return data address
\ a3

: (3array) ( n1 n2 n3 a1 a2 - a3)   >r dup >r  [ 3 cells ] literal +
   @  *  swap r@ [ 2 cells ] literal + @  *  +  swap r> [ 1 cells ] literal
   +  @  *  +  r>  + ;


\ give instance address a2, TRA a1; return bitfield address a3, mask n1
\ and start bit position n2

: (bitfield) ( a1 a2 - a3 n1 n2)   swap dup  [ 1 cells ] literal + @  swap 
   [ 2 cells ] literal + c@ ;


\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
\ \								\
\ \	Defining Words						\
\ \								\
\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

\ Compile:
\ given the address of a type, create a simple instance
\ Run:
\ given the address of the instance record a1; return the address of the
\ instance data a2

: simple-instance: ( a _)   create   @ dup , here swap dup allot  erase 
   does> ( a1 - a2)   \ should be:  dup [ 1 cells ] literal +  (simple) ;
     [ 1 cells ] literal + ;	\ but this is much quicker

\ Compile:
\ given the address of a type and a current offset, create a simple element
\ and return the adjusted offset
\ Run:
\ given the data address a1 and the element record address a2, return the
\ address of the live data a3

: simple-element: ( n a _ - n')   create  >r  /bitfield  r> @ dup , over , +
   does> ( a1 a2 - a3)
   \ should be: dup [ 1 cells ] literal + @  rot +   (simple) ;
     [ 1 cells ] literal + @ + ;  \ but this is much quicker

\ Compile:
\ given the current offset n within the type being defined and the TRA for
\ the host type of the bitfield, create a bitfield element with the name
\ in the input stream.  Return the adjusted offset n if the type is not
\ the host type is not the same length or there are not enough bits left
\ in the host type else leave n unchanged.
\ Run:
\ given the address of the instance a1 and the Element Record Address (ERA)
\ a2; return the instance address a3, the bitmask n1, and the start bit number
\ n2.


: bitfield-element: ( n a _ - n')    create
   @   dup bitfield-length - 	\ has bitfield length changed?
   (bits) dup >r  bits-left 0<	\ are there not enough bits left in this type?
   or  if			\ at this point: ( offset len)  (r: bits)
     swap  /bitfield  swap	\ advance offset value and reinitialize
     [ 1 cells ] literal min  dup to bitfield-length
   then  ,  			\ the size of the type containing bitfield
   r@   next-bit dup  r>  +  to next-bit	\ ( offset bits nextbit)
   dup >r  >mask ,   r> c,	\ the mask and start bit location
   dup ,   0 to (bits)		\ and the offset; 0 (bits) for next time
   does> ( a1 a2 - a3 n1 n2)  dup   [ 2 cells 1+ ] literal + @  rot +
   (bitfield) ;

\ Compile:
\ given the address of a type a, create a one dimensional instance
\ Run:
\ given the address of the instance record a1, the index n; return the
\ address of the instance data a2.

: 1array-instance: ( a _ - )   create  @  dup  [ 0 subscript ] literal @  *
   dup >r ,  ,   here   r@ allot   r> erase
   does> ( n a1 - a2) dup [ 2 cells ] literal +  (1array) ;

\ Compile:
\ given the address of the type a and current offset n; from the input
\ stream create a single dimensioned element returning the adjusted offset
\ Run:
\ given the subscript n, the address of the live data a1, and the address
\ of the instance record a2, return the address of the desired array item

: 1array-element: ( n a _ - n')   create   @  [ 0 subscript ]  literal @
   over *  dup >r ,   ,  dup ,  r> +
   does> ( n a1 a2 - a3)  dup [ 2 cells ] literal + @  rot +  (1array) ;

\ Compile:
\ given the address of a type a, create a two dimensional instance
\ Run:
\ given the address of the instance record a1, the indexes n1 and n2;
\ return the address of the desired item a2

: 2array-instance: ( a _)   create  @   [ 1 subscript ] literal @  over *
   [ 0 subscript ] literal @  over *   dup >r  , , ,  here r@ allot  r> erase
   does> ( n1 n2 a1 - a2)  dup [ 3 cells ] literal +  (2array) ;

\ Compile:
\ given the address of a type a and a current offset n; from the
\ input stream create a two dimension element returning the adjusted
\ offset
\ Run:
\ given the subscripts n1 and n2, the address of the live data a1, and the
\ address of the element record a2, return the address of the desired array
\ item a3

: 2array-element: ( n a _ - n')   create  @  [ 1 subscript ] literal @
   over  *  [ 0 subscript ] literal @  over *  dup >r  , , ,  dup , r> + 
   does> ( n1 n2 a1 a2 - a3)  dup [ 3 cells ] literal + @  rot + (2array) ;

\ Compile:
\ given the address of type a, create a three dimensional instance
\ Run:
\ given the address of the instance record a1, the indexes n1, n2 and n3;
\ return the address of the desired item a2

: 3array-instance: ( a _)   create  @   [ 2 subscript ] literal @  over *
   [ 1 subscript ] literal @  over *  [ 0 subscript ] literal @
   over *  dup >r  , , , ,   here r@ allot  r> erase
   does> ( n1 n2 n3 a1 - a3)   dup [ 4 cells ] literal +  (3array) ;

\ Compile:
\ given the address of type a, and a current offset n; from the input
\ stream create a three dimension element returning the adjusted offset
\ Run:
\ given the subscripts n1 n2 and n3, the address of the live data a1,
\ return the address of the desired array item a2.

: 3array-element: ( n a _ - n')   create  @ [ 2 subscript ] literal @
   over *  [ 1 subscript ] literal @  over *  [ 0 subscript ] literal @
   over *  dup >r  , , , ,  dup ,  r> + 
   does> ( n1 n2 n3 a1 - a2)   dup  [ 4 cells ]  literal + @ rot +  (3array) ;

\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
\ \								\
\ \	Integration						\
\ \								\
\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

\ Compile:
\ given the offset in the case of defining a type, nothing in the case
\ of creating an instance, based upon the contents of the subscript array
\ and the defining value call the proper defining word above for arrays
\ Run:
\ varies depending on the above

: struct: ( _)   create  here 0 0 ,  true to defining
   does> ( [n] a)
	defining if  ( n a)
		(bits) if
			bitfield-element:
		else
			swap /bitfield swap
			#subscripts case
				0 of	simple-element:		endof
				1 of	1array-element:		endof
				2 of	2array-element:		endof
				3 of	3array-element:		endof
			endcase
		then
	else  ( a)
		(bits) abort" ? Cannot define bitfield outside struct def"
		swap /bitfield swap
		#subscripts case
			0 of	simple-instance:	endof
			1 of	1array-instance:	endof
			2 of	2array-instance:	endof
			3 of	3array-instance:	endof
		endcase
	then
   /array
;

: ;struct ( a n)   /bitfield  swap !   false to defining ;


: sizeof: ( _ - n)   [compile] ' >body @   [compile] literal ; immediate

: (sub-array-sizeof:) ( n _)   create   cells  c,  immediate
   does> ( n a _ - n)   c@ swap -   [compile] ' >body + @   state @ if
   [compile] literal  then ;

0 cells constant item	immediate
1 cells constant row 	immediate
2 cells constant plane	immediate

1 (sub-array-sizeof:) 1xSizeof:
2 (sub-array-sizeof:) 2xSizeof:
3 (sub-array-sizeof:) 3xSizeof:


\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
\ \								\
\ \	Example of Use						\
\ \								\
\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

\ The current accumulated size of the structure is on the stack.
\ With simple types the address of the data area is pointed to
\ directly when instances created by the type is run.  With complex
\ types one or more subscripts must be passed to the instance word
\ to point to the proper item.  Pointers to complex types (that have
\ been malloced for instance) must be implemented as elements. Through
\ this mechanism every kind of C datastructure should be able to be
\ implemented easily.

	struct: int:	1 cells +	;struct
	struct: byte:	1 +		;struct
	struct: short:	1 cells 2/ +	;struct
	struct: double:	8 +		;struct
	struct: single:	8 +		;struct
	struct: ptr:	1 cells +	;struct


\ 	struct: point:		\ as in a graphical point on a screen
\			short:		x
\			short:		y
\		4 bit	byte:		color
\		4 bit	byte:		intensity
\			ptr: ( point: )	next.point
\	;struct
\
\	struct: line:		\ a line segment
\			point:		start
\			point:		end
\			byte:		priority
\			byte:		color
\			ptr: ( line: )	next.line
\	;struct
\
\
\	line: top  line: bot  line: left  line: right
\
\			10 10 top start >r  r@ y w!  r> x w!
\			300 500 bot start >r  r@ y w!  r> x w!
\			10 300 left start >r  r@ y w!  r> x w!
\			....
\
\	bot  top next.line !   left  bot next.line !
\	right  left next.line !   top  right next.line !
\
\	bot next.line @ start x w@ .   10 ok
\
\

\ THIS FILE HAS NOT BEEN TRUNCATED
