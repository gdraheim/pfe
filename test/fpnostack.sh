#! /bin/sh
testcase="fpnostack"

echo "
//d
/^\$/d
/^fpnostack floating point word set */!bN2
/^fpnostack floating point word set */{g;N;}
/\n======================= *\$/{g;N;}
/\n\\[ 0 \\] */{g;N;}
/\n\\[\\[ .... \\]\\] */d
:N2
/^testing F~ */!bN3
/^testing F~ */{g;N;}
/\n   0 0  0 0 0 0 0  0 0 0 0 0  0 0 */d
:N3
/^floating point number input */!bN4
/^floating point number input */{g;N;}
/\n   0 0 0 0 0 */{g;N;}
/\n   0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0  0 0  */d
:N4
/^floating point constants and variables */!bN5
/^floating point constants and variables */{g;N;}
/\n   0 0 0 0 0 0  0 0 0 0 0 0  0 0  0 0 0 0 0  0 0 */d
:N5
/^floating point stack operations */!bN6
/^floating point stack operations */{g;N;}
/\n   0 0 0  0 0 0  0 0 0  0 0 0  0 0 0 */d
:N6
/^testing REPRESENT */!bN7
/^testing REPRESENT */{g;N;}
/\n   0 0 >0<  0 0 >000<  0 0 >0000000<  0 0 >00000000000< */{g;N;}
/\n   0 1 >1<  0 1 >100<  0 1 >1000000<  0 1 >10000000000< .*/{g;N;}
/\n   0 2 >1<  0 4 >100<  0 6 >1000000<  0 12 >10000000000< */{g;N;}
/\n   0 2 >1<  0 3 >999<  0 6 >1000000<  0 30 >999999990< */{g;N;}
/\n   0 0 >1<  0 -3 >999<  0 -4 >1000000<  0 -30 >999999990< */{g;N;}
/\n   0 0 >1<  0 -3 >999<  0 -5 >9999999<  0 -30 >999999990< */{g;N;}
/\n   0 10 >1<  0 12 >123<  0 14 >1234568<  0 39 >123456789< */{g;N;}
/\n   0 8 >1<  0 6 >123<  0 4 >1234568<  0 -21 >123456789< */d
:N7
/^testing FROUND */!bNX
/^testing FROUND */{g;N;}
/\n   1.00* 2.00* -1.00* -2.00* */{g;N;}
/\n   0.00* 2.00* 2.00* 4.00*  -*0.00* -2.00* -2.00* -4.00* */{g;N;}
/\n   4000000000 4000000002 -4000000000 -4000000002 */{g;N;}
:NX
/\n   0 0 */d
/----------*/d
{p;d;}
" >$testcase.sed

sed -n -f $testcase.sed $* >$testcase.tmp
cnt=`cat $testcase.tmp | wc -w`
test "0" -eq "$cnt" || echo "$* : $cnt BAD lines"
test "0" -eq "$cnt"
exit
