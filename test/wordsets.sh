#! /bin/sh

echo "
/checking .*/D
/ ^/D
//D
s/ CODE *missing //
s/ ;CODE *missing //
s/ ASSEMBLER *missing //
s/ EDITOR *missing //
s/ missing//
p
" >wordsets.sed

sed -n -f wordsets.sed $* >wordsets.tmp
cnt=`cat wordsets.tmp | wc -w`
test "0" -eq "$cnt" || echo "$* : $cnt BAD lines"
test "0" -eq "$cnt"
exit


