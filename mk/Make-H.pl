package pfe::Make_H ;
use strict ;                        
use POSIX qw/strftime/ ;

$_ = $ARGV[0] ;                          
my $inc = "" ;                                    
/-(\w\w\w).c$/ and $inc = "<pfe/incl-$1.h>" ;
exit if not length $inc ;
my $O = $ARGV[0]; $O =~ s/\.c/\.h/ ;
my $once = $O ; $once =~ tr:a-z./-:A-Z___: ; $once =~ s:_+:_:g ;
print $once ;

my $prg = $0; s{.*/([^/]*)$}{$1};

my $out = "" ;
$out .= "#ifndef ".$once."\n" ;
$out .= "#define ".$once." ".(time)."\n" ;
$out .= "/* generated"
    .(strftime " %Y-%m%d-%H%M ", localtime)
    .$prg." ".join(" ",@ARGV)." */\n\n" ;
my $F = $ARGV[0] ;                       
open F,"<$F" or die $F, $! ; $_ = join("", <F>); close F ;

my $ext = "" ;
s{  (\#ifn?def\s+_export\s+ (?:\#define\s+_export\s+)?) 
	((?:.(?!\#endif))*.) 
	    (\#endif)  
}{                                              
    $ext .= $2;                         
    ""
}gsex ;
$out .= "#include $inc"."\n" if $ext !~ /^\s*\#\s*include\s*[\<\"]/ ;
$ext =~ s{ ( \#\s*include\s*[\<\"][^<>\"]*[\"\>]\s* ) } 
	 { $out .= $1; "" }gsex ;
s{ ^ ( \/\*\*\s (?:.(?!\*\/))* .\*\/ ) } { $out .= "\n".$1."\n"; "" }sex ;
$out .= "\n#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n" ;
$out .= "\n".$ext."\n" ;
s{ (\/\*\*\s(?:.(?!\*\/))*.\*\/)?
       (\s*(?:_export|static|extern)?\s)
	   \b (FCode|P4_CODE) (\s*\(\s* \w+ \))      
}{                                              
    my ($a,$b,$c,$d) = ($1,$2,$3,$4);    
    if ($b !~ /(static|extern)\b/) 
    {                                            
	$b =~ s/_export/_extern/ ;
        $b = $b." extern " 
	    if not $b =~ /extern\s*$/ ;
	$b =~ s/^\s*/\n/ ;
	$b =~ s/\s*$/ / ;
	$c = "P4_CODE" ;
	$d =~ s/\s+/ /gs ;
	$a = "\n".$a if length $a ;
        $out .= $a.$b.$c.$d.";\n" ;             
    };                                           
    ""                                          
}gsex ;
s{ (/\*\*\s(?:.(?!\*\/))*.\*\/)?
       (\s*_export) \b ([^;{}=]+) ([;{}=])               
}{                                              
    my ($a,$b,$c,$d) = ($1,$2,$3,$4) ;            
    if ($b !~ /(static|extern)$/)               
    {                                            
	$b =~ s/_export/_extern/ ;
	$b = $b." extern "
	    if not $b =~ /extern$/ ;
	$b =~ s/^\s*/\n/ ;
	$b =~ s/\s*$/ / ;
	$c =~ s/\s+/ /gs ;                      
	$d = "; /*".$d."*/" ;                  
	$a = "\n".$a if length $a ;
	$out .= $a.$b.$c.$d."\n" ;                 
    } ;                                           
    ""
}gsex ;
$out .= "\n#ifdef __cplusplus\n} /* extern \"C\" */\n#endif\n\n" ;
$out .= "\n#endif\n" ;
open O,">$O" or die $O, $! ; print O $out; close O ;
print " ",$out =~ s/extern//g," extern\n" ;
1

