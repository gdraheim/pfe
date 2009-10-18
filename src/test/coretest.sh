#! /bin/sh
testcase="coretest"

echo "
/is redefined/d
//d
/YOU SHOULD SEE THE STANDARD GRAPHIC CHARACTERS:/b graphicchars
/YOU SHOULD SEE 0-9 SEPARATED BY A SPACE:/b onespacedigits
/YOU SHOULD SEE 0-9 (WITH NO SPACES):/b nospacedigits
/YOU SHOULD SEE A-G SEPARATED BY A SPACE:/b onespacealpha
/YOU SHOULD SEE 0-5 SEPARATED BY TWO SPACES:/b twospacealpha
/YOU SHOULD SEE TWO SEPARATE LINES:/b twoseperatelines
{p;d;}
:graphicchars
# actually ASCII chars...
{N;N;N;s/YOU SHOULD SEE THE STANDARD GRAPHIC CHARACTERS://;}
s|!\"\#\$%&'()\*+,\-\.\/0123456789:;<=>?@||
s/ABCDEFGHIJKLMNOPQRSTUVWXYZ\[\\\\\]\^\_\`//
s/abcdefghijklmnopqrstuvwxyz{|}\~//
{p;d;}
:onespacedigits
{N;s/YOU SHOULD SEE 0-9 SEPARATED BY A SPACE://;}
s/0 1 2 3 4 5 6 7 8 9 //
{p;d;}
:nospacedigits
{N;s/YOU SHOULD SEE 0-9 (WITH NO SPACES)://;}
s/0123456789//
{p;d;}
:onespacealpha
{N;s/YOU SHOULD SEE A-G SEPARATED BY A SPACE://;}
s/A B C D E F G //
{p;d;}
:twospacealpha
{N;s/YOU SHOULD SEE 0-5 SEPARATED BY TWO SPACES://;}
s/0  1  2  3  4  5  //
{p;d;}
:twoseperatelines
{N;N;s/YOU SHOULD SEE TWO SEPARATE LINES://;}
s/LINE 1.*\n.*LINE 2//
{p;d;}
" >$testcase.sed

sed -n -f $testcase.sed $* >$testcase.tmp
cnt=`cat $testcase.tmp | wc -w`
test "0" -eq "$cnt" || echo "$* : $cnt BAD lines"
test "0" -eq "$cnt"
exit
