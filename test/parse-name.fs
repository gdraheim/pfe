require test/tester.fs

{ PARSE-NAME abcd S" abcd" COMPARE -> 0 }
{ PARSE-NAME   abcde   S" abcde" COMPARE -> 0 }
\ test empty parse area
{ PARSE-NAME
  NIP -> 0 }
{ PARSE-NAME   
  nip -> 0 }

{ : parse-name-test ( "name1" "name2" -- n )
    PARSE-NAME PARSE-NAME COMPARE ; -> }
{ parse-name-test abcd abcd -> 0 }
{ parse-name-test  abcd   abcd   -> 0 }
{ parse-name-test abcde abcdf -> -1 }
{ parse-name-test abcdf abcde -> 1 }
{ parse-name-test abcde abcde
  -> 0 }
{ parse-name-test abcde abcde  
  -> 0 }

