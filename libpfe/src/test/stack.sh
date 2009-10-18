#! /bin/sh
testcase="stack"

echo "
//d
/^\$/d
/^stack operators.*/d
/^single co.*/!bN2
/^single co.*/{g;N;}
/\n11 0/{g;N;}
/\n11 0/{g;N;}
/\n22 22 11 0/{g;N;}
/\n22 22 11 0/{g;N;}
/\n22 22 11 0/{g;N;}
/\n22 22 11 0/{g;N;}
/\n0 11 0/{g;N;}
/\n0 11 0/{g;N;}
/\n11 22 0/{g;N;}
/\n11 22 0/{g;N;}
/\n11 22 11 0/{g;N;}
/\n11 22 11 0/{g;N;}
/\n11 33 22 0/{g;N;}
/\n11 33 22 0/{g;N;}
/\n22 0/{g;N;}
/\n22 0/{g;N;}
/\n22 11 22 0/{g;N;}
/\n22 11 22 0/{g;N;}
/\n22 11 33 0/{g;N;}
/\n22 11 33 0/d
:N2
/^double co.*/!bNX
/^double co.*/{g;N;}
/\n11 0/{g;N;}
/\n11 0/{g;N;}
/\n22 22 11 0/{g;N;}
/\n22 22 11 0/{g;N;}
/\n11 22 0/{g;N;}
/\n11 22 0/{g;N;}
/\n11 22 11 0/{g;N;}
/\n11 22 11 0/{g;N;}
/\n11 33 22 0/{g;N;}
/\n11 33 22 0/d
:NX
/----------*/d
{p;d;}
" >$testcase.sed

sed -n -f $testcase.sed $* >$testcase.tmp
cnt=`cat $testcase.tmp | wc -w`
test "0" -eq "$cnt" || echo "$* : $cnt BAD lines"
test "0" -eq "$cnt"
exit
