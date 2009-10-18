\
\ test/exception.4th --- test forth signals (orig.: tests the exception word set)
\ (duz 10Aug93)
\

[DEFINED] D [NOT] [IF] : D DEPTH . ; [THEN]
[DEFINED] Y [NOT] [IF] : Y IF ." /Y/ " ELSE ." BAD/Y/ " THEN ; [THEN]
[DEFINED] N [NOT] [IF] : N IF ." BAD/N/ " ELSE ." /N/ " THEN ; [THEN]

CR .(  Test handling signals with Forth words. ) CR

: SIG-TERM-HDL ." SIGTERM handled ok. " ;
' SIG-TERM-HDL SIGTERM FORTH-SIGNAL .
SIGTERM RAISE-SIGNAL
0 SIGTERM FORTH-SIGNAL ' SIG-TERM-HDL = Y

\ (guidod: this seems to have been working at some time but it fails now, ouch)
