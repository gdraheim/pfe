 $_ = join("",<>); if ( m:  \s*3\s*\s*2\s*\s*1\s*\s*1\s*\s*2\s*\s*  :sx ) {  print "OK" ; } else { print "FAIL" ; }
