
: SECS TIME&DATE  DROP DROP DROP  60 * + 60 * + ;

create buf 60 allot

: holds ( str len -- )
  0 do dup i + c@ hold loop drop 
;

: third 2 pick ;

: create-comptest.fs ( -- 0 | errorstat )
  s" comptest.fs" W/O create-file
  ?dup if ." could not open comptest.fs for testpurpose " nip exit then
  30000 0 do 
	i 0 <# s" ; pord pud " holds #S s" x : " holds #>
	third write-line drop
  loop
  close-file
;

create-comptest.fs drop
secs
s" comptest.fs" included
secs swap - cr . .( secs)
\ s" comptest.fs" delete-file drop
bye


