
: sec@ gettimeofday nip ;

variable duration
: x sec@ swap begin dup while 1 - repeat drop sec@ swap - duration ! ;

cr .( 1/4mio x = )     250000 x    duration ?
cr duration @ 10 < [IF]
    .( 1mio x = )    1000000 x   duration ?
[ELSE] .( 1mio x skipped-) [THEN]
cr duration @ 10 < [IF]
    .( 4mio x = )    4000000 x   duration ?
[ELSE] .( 4mio x skipped-) [THEN]
cr duration @ 10 < [IF]
    .( 16mio x = )   16000000 x   duration ?
[ELSE] .( 16mio x skipped-) [THEN]
cr duration @ 10 < [IF]
    .( 64mio x = )   64000000 x  duration ?
[ELSE] .( 64mio x skipped-) [THEN]
cr duration @ 10 < [IF]
    .( 256mio x = )  256000000 x duration ?
[ELSE] .( 256mio x skipped-) [THEN]
cr duration @ 10 < [IF]
    .( 1024mio x = )  1024000000 x duration ?
[ELSE] .( 1024mio x skipped-) [THEN]
cr

