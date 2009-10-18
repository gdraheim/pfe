#! /bin/sh
testcase="string"

echo "
//d
/^\$/d
/^string word set\$/!bskip
/^string word set\$/{g;N;}
/\n===============\$/{g;N;N;}
/\n\ndefine \$/{g;N;}
/\ncompare .Y. .Y. .Y. .Y. .Y.  0 $/{g;N;}
/\nsearch .Y. .Y. .Y. .Y. .N. .Y.  0 $/d
:skip
/----------*/d
{p;d;}
" >$testcase.sed

sed -n -f $testcase.sed $* >$testcase.tmp
cnt=`cat $testcase.tmp | wc -w`
test "0" -eq "$cnt" || echo "$* : $cnt BAD lines"
test "0" -eq "$cnt"
exit
