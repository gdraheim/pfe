
\ how good is the hashfunction in use.

: hashlink-depth ( nfa -- depth )
  0 swap 
  begin ?dup while 
   swap 1+ swap 
   name> >link @ 
  repeat 
;

: hashvoc-distri ( -- )
  31 0 do 
    context @ i cells + @ 
    hashlink-depth . 
    i 15 and 0= if cr then
  loop ;

: run hashvoc-distri ;

\ background:
\ in pfe, words are memorized case-sensitive but 
\ the wordset itself can be case-insensitive. Therefore
\ the hashvalue for a name must be the same for all
\ words that only differ in case for a few positions.
\ Currently, I use ( h += *s++ ; h ^= l++ ; ) and the
\ hashsize is limited to 32-modulo - it's just about
\ the lower 5-bits of ascii-chars to match for this
\ case. A greater hashmodulo would need to have it
\ forcebly made case-insensitive, e.g.
\ ( h += 0x40 | *s++ ; h ^= l++ ; )

