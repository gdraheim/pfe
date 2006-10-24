package pfe::Make_H ;
use strict ;                        
use POSIX qw/strftime/ ;

my $helpstring = " <*.c file>\n"
    ."\t scan the C file in the pfe/* source directory and regenerate\n"
    ."\t the header file derived from them. The script will look for:\n"
    ."\t - a global comment block at the start of the file to be copied\n"
    ."\t - FCode(name) definitions unless they are marked 'static'\n"
    ."\t - a preceding comment block before that FCode extern\n"
    ."\t - any C declaration preceded by '_extern' copied to 'extern'\n"
    ."\t - a preceding comment block before that _extern prototope\n"
    ."\t - multiple lines between #ifndef _extern...#endif\\n with the\n"
    ."\t   ifndef removed. Header #includes are moved to the top of the.\n"
    ."\t   generated output file right after the initial #include\n"
    ."\t The body of each regenerated header file is wrapped in the\n"
    ."\t usual #ifndef once_declare...#endif derived from the filename\n"
    ."\t of the input/output file. The output file has just the .h ext\n"
    ."\t replaced with a .c extension. Additionally each header file has\n"
    ."\t an 'initial include' file required to typedef the most basic\n"
    ."\t pfe idioms including the #define FCode(). In pfe each of the\n"
    ."\t c source files has a two part stem worset-ext.c and the part\n"
    ."\t after the last hyphen determines the variant of the initial\n"
    ."\t include file - pfe-sub.h, pfe-ext.h, pfe-mix.h etc.\n";
if ($#ARGV <= -1) { print $0, $helpstring ; exit 1; }

my $pfe = $ARGV[0]; $pfe =~ s{^(.*)/[^/]*$}{$1};

$_ = $ARGV[0] ;
my $inc = "" ;                               
/-(\w\w\w).c$/   and -s "$pfe/incl-$1.h" and $inc = "<pfe/incl-$1.h>" ;
/-(\w\w\w).c$/   and -s "$pfe/pfe-$1.h"  and $inc = "<pfe/pfe-$1.h>" ;
/-(\w\w\w\w).c$/ and -s "$pfe/def-$1.h"  and $inc = "<pfe/def-$1.h>" ;
/-(\w\w\w\w).c$/ and -s "$pfe/pfe-$1.h"  and $inc = "<pfe/pfe-$1.h>" ;
if (not length $inc) { 
    print STDERR $_," does not have matching base include (using pfe-base)\n";
    $inc = "<pfe/pfe-base.h>";
}
my $O = $ARGV[0]; $O =~ s/\.c/\.h/ ;
my $once = $O ; $once =~ tr:a-z./-:A-Z___: ; $once =~ s:_+:_:g ;
print $once ; print " " x (27 - length $once) if length $once < 27;

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
	   \b (FCode|FCode_XE|FCode_RT|P4_CODE) (\s*\(\s* \w+ \))      
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
my $count = ($out =~ s/extern//g);
print " " x (4 - length $count),$count," extern       ", $inc, "\n" ;
1

