#! /bin/sh
testcase="checkans"

echo "
//d
/^ *\$/d
/^Checking .*\$/d
/^Missing: \\;CODE ASSEMBLER CODE EDITOR *\$/d
/^Missing: ASSEMBLER EDITOR *\$/d
/^Missing: EDITOR *\$/d
/^Wordset: *Status: *Words: */!bNX
/^Wordset: *Status: *Words: */{g;N;}
/\n11 0/{g;N;}
/\nCORE           *complete 133 \$/{g;N;}
/\nCORE-EXT       *complete 46 \$/{g;N;}
/\nBLOCK          *complete 8 \$/{g;N;}
/\nBLOCK-EXT      *complete 6 \$/{g;N;}
/\nDOUBLE         *complete 20 \$/{g;N;}
/\nDOUBLE-EXT     *complete 2 \$/{g;N;}
/\nEXEPTION       *complete 2 \$/{g;N;}
/\nEXEPTION-EXT   *complete 2 \$/{g;N;}
/\nFACILITY       *complete 3 \$/{g;N;}
/\nFACILITY-EXT   *complete 6 \$/{g;N;}
/\nFILE           *complete 21 \$/{g;N;}
/\nFILE-EXT       *complete 4 \$/{g;N;}
/\nFLOATING       *complete 31 \$/{g;N;}
/\nFLOATING-EXT   *complete 41 \$/{g;N;}
/\nLOCAL          *complete 2 \$/{g;N;}
/\nLOCAL-EXT      *complete 1 \$/{g;N;}
/\nMEMORY         *complete 3 \$/{g;N;}
/\nTOOLS          *complete 5 \$/{g;N;}
/\nTOOLS-EXT      *partial  9 . 13 \$/{g;N;}
/\nTOOLS-EXT      *partial  11 . 13 \$/{g;N;}
/\nTOOLS-EXT      *partial  12 . 13 \$/{g;N;}
/\nSEARCH         *complete 9 \$/{g;N;}
/\nSEARCH-EXT     *complete 5 \$/{g;N;}
/\nSTRING         *complete 8 \$/d
:NX
/----------*/d
{p;d;}
" >$testcase.sed

sed -n -f $testcase.sed $* >$testcase.tmp
cnt=`cat $testcase.tmp | wc -w`
test "0" -eq "$cnt" || echo "$* : $cnt BAD lines"
test "0" -eq "$cnt"
exit
