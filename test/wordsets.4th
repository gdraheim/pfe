
only forth definitions
: WS CR BL WORD ." checking wordset " COUNT TYPE CR ;

: ^ BL WORD 
  FIND IF SPACE ASCII ^ EMIT 
  ELSE CR SPACE COUNT TYPE SPACE ." missing" SPACE CR THEN
;

include wordsets.fs
