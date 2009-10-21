#! /usr/bin/perl
use strict;

my $T = "";

my %opt;
my $sep = " -"; # "...";

my %ignorefiles;
my $i;

for (@ARGV)
{
    if (/^--?X=(.*)/) { $ignorefiles{$1} = "! "; next; }
    if (/^--?(\w+)=(.*)/) { $opt{$1} = $2; next; }
    if (/^--?(\w[\w_]*)$/) { $opt{$1} = "*"; next; }

    my $ignore="";
    for $i (keys %ignorefiles) { if (/\/$i/) { $ignore=$i; }; }
    if ($ignore) { print STDERR "(ignored $_)"; next; }

    my $F = $_;
    if (not open F, "< $F") { print STDERR "could not open $F: $!\n"; next; }
    $T .= join ("", <F>);
    close F;
}

my $refentryinfo = "";
if ($opt{infodate} and $opt{package})
{  $refentryinfo =
       "<date>".$opt{dateinfo}."</date>\n"
       ."<productname>".$opt{package}."</productname>\n";
}

$T =~ s{</pfewordlists>}{</chapter>\n};
$T =~ s{<pfewordlists>}
{<chapter id=\"wordsets\"><title>Wordset Reference</title>\n};
# $T =~ s{</?pfewordlist\b[^<>]*>}{}gs;
$T =~ s{<pfewordlist\b[^<>]*\swordset=\"([^<>\"]*)\"[^<>]*>}
{<sect1 id=\"wordset::$1\"><title>$1 wordset</title>\n<variablelist>}gs;
$T =~ s{</pfewordlist\b[^<>]*>}{</variablelist></sect1>\n\n}gs;

$T =~ s:</br>::sg;
$T =~ s:<br\b[^<>]*>::sg;
$T =~ s:</?J\b[^<>]*>:  :sg;

# $T =~ s{<cblock>((?:.(?!</?cblock\b))*.)</cblock>}{}sg;

# ---------------------------------------------------------------------
# dpans helper
my %dpans = ( );

sub dirname
{    my $in = $_[0];
    if ($in !~ /\//) { return "."; }else{ $in =~ s{/[^/]*$}{}x ; return $in; }
}

sub dpans_read
{
    my ($F, $dir) = @_;
    my $line;
    $F = "dpans" if not length $F;

    if ( -d $F )  {
        if ( -f "$F/dpansf.html" )      { $F .= "/dpansf.html"; }
        elsif ( -f "$F/dpansf.htm" )    { $F .= "/dpansf.htm"; }
        else { return ""; } # oh no
    }
    if (not length $dir)  {
        if (defined $opt{dpans} and length $opt{dpans}) { $dir = $opt{dpans}; }
        else { $dir = dirname($F); }
    }
    if ( ! -f $F )  {
        if ( -f "$dir/dpansf.html" )      { $F = "$dir/dpansf.html"; }
        elsif ( -f "$dir/dpansf.htm" )    { $F = "$dir/dpansf.htm"; }
        else { return ""; } # oh no
    }

    return "" if not open F, "<$F";
    print STDERR "<reading $F>\n";

    while ($line = <F>)
    {
#        if ($line =~ m{^\s+
#                 (\<A \s+ href=dpans\d+\.html\#[\d\.]+\> [\d\.]+ \<\/A\>)
#                     \s+ ([^\s]+) \s }ix)
        if ($line =~ m{^\s+
   (<A \s+ href=dpans\d+\.html?\#[\d\.]+>  [\d\.]+ </A>) \s+  (\S+) }ix)
        {
            my $ref = $1;
            my $key = $2;
	    my $fileref = "file:";
	    $fileref = "" if length $opt{nofileref};

            $key =~ s{<B>.*</B>} {};
            $key =~ s{\"}{\&quot\;}gs;

            $ref =~ s{href=(dpans[^<>]*)} {url=\"$fileref$dir/$1\"};
            $ref =~ s:<A:<ulink:;
            $ref =~ s:</A:</ulink:;

	    $dpans{$key} = $ref;
        }
    }
}

&dpans_read if $opt{indexterm};

# ---------------------------------------------------------------------

sub fcodename
{
    local $_ = lc($_[0]);
    s,-,-minus-,g;
    s,\&amp;,-and-,g;
    s, ,-_-,g;
    s,^\>,-to-,g;    s,^\&lt\;,-to-,g;
    s,\>,-from-,g;   s,\&lt\;,-from-,g;
    s,\<,-back-,g;   s,\&gt\;,-back-,g;
    s,\@,-fetch-,g;
    s,\!,-store-,g;
    s,\",-quote-,g;  s,\&quot\;,-quote-,g;
    s,\',-tick-,g;
    s,\\,-backslash-,g;
    s,\#,-sharp-,g;
    s,\+,-plus-,g;
    s,\~,-like-,g;
    s,\=,-equal-,g;
    s,\/,-slash-,g;
    s,\:,-colon-,g;
    s,\*,-star-,g;
    s,\.,-dot-,g;
    s,\(\((.*)\)\),-double-paren-$1,;
    s,\[\[(.*)\]\],-double-bracket-$1,;
    s,\((.*)\),-paren-$1,;
    s,\[(.*)\],-bracket-$1,;
    s,\{$,$1-start,;
    s,^\},$1-ends,;
    s,\(,$1-note,;
    s,\),$1-done,;
    s,\?,-question-,g;
    s,\$,-str-,g;
    s,\%,-percent-,g;
    s/\,/-comma-/g;
    s,\;,-semicolon-,g;

    if (m,(\w*)([^\w_-]+)(\w*),) { print STDERR "\"".$1."{".$2."}".$3."\""; }

    s{ 0(\d) } {-o~$1-}gsx;
    s{ 1(\d) } {-ten~$1-}gsx;
    s{ 2(\d) } {-twenty~$1-}gsx;
    s{ 3(\d) } {-thirty~$1-}gsx;
    s{ 4(\d) } {-fourty~$1-}gsx;
    s{ 5(\d) } {-fifty~$1-}gsx;
    s{ 6(\d) } {-sixty~$1-}gsx;
    s{ 7(\d) } {-seventy~$1-}gsx;
    s{ 8(\d) } {-eighty~$1-}gsx;
    s{ 9(\d) } {-ninety~$1-}gsx;
    s{ -o~0- } {-hundred-}gsx;
    s{ -ten~1- } {-eleven-}gsx;
    s{ -ten~2- } {-twelve-}gsx;
    s{ -ten~3- } {-thirteen-}gsx;
    s{ -ten~4- } {-fourteen-}gsx;
    s{ -ten~5- } {-fifteen-}gsx;
    s{ -ten~6- } {-sixteen-}gsx;
    s{ -ten~7- } {-seventeen-}gsx;
    s{ -ten~8- } {-eightteen-}gsx;
    s{ -ten~9- } {-nineteen-}gsx;
    s{ ~0 } {}gsx;
    s{ ~1 } {one}gsx;
    s{ ~2 } {two}gsx;
    s{ ~3 } {three}gsx;
    s{ ~4 } {four}gsx;
    s{ ~5 } {five}gsx;
    s{ ~6 } {six}gsx;
    s{ ~7 } {seven}gsx;
    s{ ~8 } {eight}gsx;
    s{ ~9 } {nine}gsx;
    s{ 0 } {-zero-}gsx;
    s{ 1 } {-one-}gsx;
    s{ 2 } {-two-}gsx;
    s{ 3 } {-three-}gsx;
    s{ 4 } {-four-}gsx;
    s{ 5 } {-five-}gsx;
    s{ 6 } {-six-}gsx;
    s{ 7 } {-seven-}gsx;
    s{ 8 } {-eight-}gsx;
    s{ 9 } {-nine-}gsx;

    s,^-,,;
    s,[^\w_]+,-,g;
    s,-$,,g;

    return $_;
}

my %nostacknotation; # words without a stack notation

my %link;    # how to resolve <link>s

my %V;

# my $id = 100; # this is the old variant - in the new variant we use a
# hash as $id{$forth_name}{$wordset} to carry an integer that makes
# forth_names unique.
my %id;
sub pfeworditem
{
    my $X = $_[0];
    my $O = "";
    my ($forth_name,$forth_stack,$forth_hints);
    my $fcode_name;
    my $doc;
    my ($forth_stack,$forth_wordlist,$export_type);
    my $wordset;
    if ($X =~ m{ forth_wordlist=\"([^\"]*)\"}s) { $forth_wordlist = $1; ""}
    if ($X =~ m{ wordset=\"([^\"]*)\"}s) { $wordset = ":$1"; }
    if ($X =~ m{ fcode_name=\"([^\"]*)\"}s) { $fcode_name = $1; ""}
    if ($X =~ m{ forth_name=\"([^\"]*)\"}s) { $forth_name = $1; ""}
    if ($X =~ m{ export_type=\"([^\"]*)\"}s) { $export_type = $1; ""}

    # we need an fcode_name that is absolut unique in order to use it
    # as both idref and filename-stem in manpage generation. The C-name
    # seen in FXCode(name) can be taken as unique, but if there is no
    # such thing (for non-prim code) then we need to generate an id from
    # the export-string: but export-strings are not necessarily unique.
    my $sec = ""; $sec = ":environment" if $forth_wordlist eq "ENVIRONMENT";
    my $serial = "";
    if (not length $fcode_name)
    {
	$fcode_name = "as-??";
	$fcode_name = "as-".&fcodename($forth_name) if (length $forth_name);
	if (length $sec) {
	    $serial .= ":";
	}else{
	    my $idnum = 0;
	    $idnum = $id{lc($forth_name)}{$wordset}
	    if exists $id{lc($forth_name)}{$wordset};

	    $idnum++; $id{lc($forth_name)}{$wordset} = $idnum;
	    print STDERR "($forth_name:$wordset:$idnum)" if $idnum != 1;
	    $serial = ".".$idnum;
	    # $serial = ".".$idnum if $idnum != 1;
	}
    }
    $wordset .= $serial.$sec;

    return if $export_type =~ / exception /;

    $X =~ s{<doc>((?:.(?!</?doc\b))*.)</doc>} { $doc = $1; ""}gsex;

    $doc =~ s{<forth_name>(.*)</forth_name>}{ $forth_name = $1; "" }sex
	if not length $forth_name;
    $doc =~ s{<forth_stack>(.*)</forth_stack>}{ $forth_stack = $1; "" }sex;
    $doc =~ s{<forth_hints>(.*)</forth_hints>}{ $forth_hints = $1; "" }sex;

    if (length $doc) { print STDERR "!"; }
    else { print STDERR "."; $nostacknotation{$forth_name} = $wordset;
	   $doc = "$forth_name ( .. )";
	   $forth_stack = "( .. )";
       }

    $doc =~ s:</?forth_\w+[^<>]*>::sg;
    my $pref = ""; my $code_name=$fcode_name; $code_name =~ y:_:-:;
    $code_name =~ s{^(\w+)-}{$pref="$1:"; ""}sex;
    my $idname = $code_name;
    $code_name = '&quot;'.$idname.'&quot;';

    # refentrytitle - goes first into TOC of html/htmlhelp
    # refpurpose - goes second into TOC of html/htmlhelp
    # refname - used by troff as filename stem !
    # refname + refpurpose - used as "NAME" section of manualpage
    my $cmd = 0;

    $O .= "<refentryinfo>".$refentryinfo
	."<title>$forth_name$sep [$wordset:]</title>"
	."</refentryinfo>\n\n" if length $refentryinfo;

    $O .= "<refmeta><manvolnum>9</manvolnum>\n"
	."<refentrytitle>$forth_name$sep [$wordset:]</refentrytitle>\n"
	."</refmeta>\n\n";
    $O .="<refnamediv>\n"
	."<refname>$idname$wordset</refname>\n"
	."<refpurpose>$export_type</refpurpose>\n"
	."</refnamediv>\n\n" if (!$cmd);
    $O .="<refnamediv>\n"
	."<refname>$idname$wordset</refname>\n"
	."<refpurpose>$export_type - $forth_wordlist</refpurpose>\n"
	."</refnamediv>\n\n" if ($cmd);

    my $funcsynopsis = "<funcsynopsis>\n"
	."<funcsynopsisinfo>$forth_wordlist</funcsynopsisinfo>\n"
	."<funcprototype>\n"
	."<funcdef><function>$forth_name</function>\n $forth_stack"
	.(length $forth_hints ? "<replaceable>$forth_hints</replaceable>":"")
	."</funcdef>\n<paramdef>$pref$code_name</paramdef>\n"
	."</funcprototype>\n</funcsynopsis>";

    $O .="<refsynopsisdiv>\n"
	."<funcsynopsisinfo>$forth_wordlist</funcsynopsisinfo>\n"
	."<funcsynopsis>\n<funcprototype>\n"
	."<funcdef><function>$forth_name</function>\n $forth_stack"
	.(length $forth_hints ? "<replaceable>$forth_hints</replaceable>":"")
	."</funcdef>\n<paramdef>$pref$code_name</paramdef>\n"
	."</funcprototype>\n</funcsynopsis>\n"
	."</refsynopsisdiv>" if (!$cmd);
    $O .="<refsynopsisdiv>\n"
	."<cmdsynopsis><command>$forth_name</command>\n"
	."<arg choice=\"plain\"> $forth_stack"
	.(length $forth_hints ? "<replaceable>$forth_hints</replaceable>":"")
	."</arg><sbr/>\n<arg choice=\"plain\">$pref$code_name</arg>"
	."</cmdsynopsis>\n".
	"</refsynopsisdiv>\n" if ($cmd);

    my $info;
    $X =~ s{<item_cblock\b[^<>]*>(.*)</item_cblock>}{ $info = $1; ""}sex;
    $X =~ s{<item_info>(.*)</item_info>}{ $info = $1; ""}sex;

    $info =~ s{ (</?)p\b([^<>]*>) }{$1."para".$2 }gsex;
    $info =~ s{ &lt;(\w+\s+/)&gt; }{<$1>}gsx;
    $info =~ s{ &lt;(\w+)&gt;([^&]+)&lt;/\1&gt; }{<$1>$2</$1>}gsx;
    $info =~ s{ &lt;(\w+)&gt;([^&]+)&lt;/\1&gt; }{<$1>$2</$1>}gsx;
    $info =~ s{ <tt>([^<>]*)</tt>}{<literal>$1</literal>}gsx;
    $info =~ s{ (</?)c> }{$1."code>"}gsex;
    $info =~ s{ (</?)small> }{$1."note>"}gsex;
    $info =~ s{ (</?)(em|i|b)> }{$1."emphasis>"}gsex;
    $info =~ s{ (>)([^<>]*)<br\s+/> }{ $1."<para>".$2."</para>" }gsex;
    $info =~ s{ (>)([^<>]*)<br\s+/> }{ $1."<para>".$2."</para>" }gsex;

    $info =~ s:</?fcode_name\b[^<>]*>::sg;
    $info =~ s:<fcode_type>.*<cblock>:<cblock>:s;
#    $info =~ s{<cblock>((?:.(?!</?cblock\b))*.)</cblock>}{}sg;
#    $info =~ s{<cblock>((?:.(?!</?cblock\b))*.)</cblock>}{}sg;
#    $info =~ s{<cblock>((?:.(?!</?cblock\b))*.)</cblock>}{}sg;
    $info =~ s{<cblock>.*</cblock>}{}sg;
    $info =~ s:(<)fcode_type\b([^<>]*>):"<para>".$1."emphasis".$2:sge;
    $info =~ s:(</)fcode_type\b([^<>]*>):$1."emphasis".$2."</para>":sge;
    $info =~ s:(</?)cblock\b([^<>]*>):$1."screen".$2:sgee;
    $info =~ s:(</?)\wliteral\b([^<>]*>):$1."literal".$2:sge;
    $info =~ s:<ccomment\b[^<>]*>(/[*]+)?::sg;
    $info =~ s:([*]+/)?</ccomment\b[^<>]*>::sg;
    $info =~ s:(</?)code\b([^<>]*>):$1."literal".$2:sge;
    $info =~ s:</?info\b[^<>]*>::sg;
    $info =~ s:</?cpreproc\b[^<>]*>::sg;

    $info =~ s{([^!=\w])=\&gt\;\&quot\;((?:[^<](?!\&quot\;))*.)(\&quot\;)}
    {$1<link>$2</link>}mg;
    $info =~ s:([^!=\w])=\&gt\;\"([^\"<>]+)(\"):$1<link>$2</link>:mg;
    $info =~ s:([^!=\w])=\&gt\;\'([^\'<>]+)(\'):$1<link>$2</link>:mg;
    $info =~ s{([^!=\w])=\&gt\;[ ]([\(\),.][^\s!?<>]*)}
    {$1<link>$2</link>}mgx;
    $info =~ s{([^!=\w])=\&gt\;[ ]
	       ([^\s<>\(\)] [^\s,.!?<>\(\)]* (?:[,.!?][A-Z][^\s,.!?<>\(\)]*)? )
		   ([\s,.!?<\(\)])}
    {$1<link>$2</link>$3}mgx;
    $info =~ s{([^!=\w])=\&gt\;[ ]
	       ([^\s<>\(\)] [^\s,.!?<>\(\)]* (?:[,.!?][A-Z][^\s,.!?<>\(\)]*)? )
		   ([\s,.!?<\(\)])}
    {$1<link>$2</link>$3}mgx;

    if ($opt{indexterm} and exists $dpans{$forth_name})
    {
	    $info .= "  <para> dpANS ".$dpans{$forth_name}."\n";
	    $info .= "  - standard forth word</para>\n";
    }

    $O .= "<refsect1><title>Description</title>\n";
    $O .= "<indexterm scope=\"local\"><primary>word</primary>\n<secondary>"
	."$forth_name$sep [$wordset:]</secondary>"
	."</indexterm>\n" if $opt{indexterm};
    $O .= $info."\n</refsect1>";

    $X = ""; $forth_name = $code_name if not length $forth_name;
    my $N = uc($forth_name)."  ".$serial.$wordset;
    $N =~ s{[&]LT\;}{<}sg;     $N =~ s{[&]QUOT\;}{\"}sg;
    $N =~ s{[&]GT\;}{>}sg;     $N =~ s{[&]AMP\;}{\&}sg;
    $N =~ s{--}{- -}sg;
    $N =~ s{^([^A-Z_][^A-Z_])([A-Z_].*)}{$2." ".$1}sex;
    $N =~ s{^([^A-Z_])([A-Z_].*)}{$2." ".$1}sex;
    $N =~ s{^([^A-Z_].*)}{".".$1}sex;

    $link{$forth_name}{$wordset} = "$idname$wordset";
    $V{$N}="\n<refentry id=\"$idname$wordset\">".$O
	."\n<!-- $N -->".$X."\n</refentry>";
    return "\n<varlistentry><term id=\"$idname$wordset:\">\n"
	.$funcsynopsis."</term>\n<listitem>".$info
	."\n<!--...-->".$X."\n</listitem></varlistentry>";
}

$T =~ s{<pfeworditem>((?:.(?!</?pfeworditem\b))*.)</pfeworditem>}
{ &pfeworditem($1) }gsex;

sub pfewordsetinfo
{
    my $info = $_[0];

    $info =~ s{ (</?)p\b([^<>]*>) }{$1."para".$2 }gsex;
    $info =~ s{ &lt;(\w+\s+/)&gt; }{<$1>}gsx;
    $info =~ s{ &lt;(\w+)\s+(\w+=)(\w+)\s*&gt;([^&]+)&lt;/\1&gt; }
		{<$1 $2\"$3\">$4</$1>}gsx;
    $info =~ s{ &lt;(\w+)&gt;([^&]+)&lt;/\1&gt; }
		{<$1>$2</$1>}gsx;
    $info =~ s{ &lt;(\w+)&gt;([^&]+)&lt;/\1&gt; }
		{<$1>$2</$1>}gsx;
    $info =~ s{ <tt>([^<>]*)</tt>}{<literal>$1</literal>}gsx;
    $info =~ s{ (</?)c> }{$1."code>"}gsex;
    $info =~ s{ (</?)small> }{$1."note>"}gsex;
    $info =~ s{ (</?)(em|i|b)> }{$1."emphasis>"}gsex;
    $info =~ s{ (>)([^<>]*)<br\s+/> }{ $1."<para>".$2."</para>" }gsex;
    $info =~ s{ (>)([^<>]*)<br\s+/> }{ $1."<para>".$2."</para>" }gsex;

    $info =~ s{(<para>)[^<>]*\@version\b[^<>]*(<)}{$1$2}; # internal SEC
    $info =~ s{(</?)(doc|info)(\b[^<>]*>)}{}gsx;
    $info =~ s{\A [/][*]+\ * (.*) [*]+[/] \Z} {$1}sx;
    $info =~ s{\s*\@description}{<emphasis>description:</emphasis>}sx;
    $info =~ s{\s*\@name\b(.*)$}{<screen>$1</screen>}mx;

    $info =~ s{([^!=\w])=\&gt\;\&quot\;((?:[^<](?!\&quot\;))*.)(\&quot\;)}
    {$1<link>$2</link>}mg;
    $info =~ s:([^!=\w])=\&gt\;\"([^\"<>]+)(\"):$1<link>$2</link>:mg;
    $info =~ s:([^!=\w])=\&gt\;\'([^\'<>]+)(\'):$1<link>$2</link>:mg;
    $info =~ s{([^!=\w])=\&gt\;[ ]([\(\),.][^\s!?<>]*)}
    {$1<link>$2</link>}mgx;
    $info =~ s{([^!=\w])=\&gt\;[ ]
	       ([^\s<>\(\)] [^\s,.!?<>\(\)]* (?:[,.!?][A-Z][^\s,.!?<>\(\)]*)? )
		   ([\s,.!?<\(\)])}
    {$1<link>$2</link>$3}mgx;
    $info =~ s{([^!=\w])=\&gt\;[ ]
	       ([^\s<>\(\)] [^\s,.!?<>\(\)]* (?:[,.!?][A-Z][^\s,.!?<>\(\)]*)? )
		   ([\s,.!?<\(\)])}
    {$1<link>$2</link>$3}mgx;

    $info =~ s{ &lt;(\w+)&gt;([^&]+)&lt;/\1&gt; }
		{<$1>$2</$1>}gsx;
    $info =~ s{ (</?)small> }{$1."emphasis>"}gsex;

    # debug-ext / signal-ext specials
    $info =~ s{\&lt\;/?d[ld]\&gt\;}{}gs;
    $info =~ s{</?d[ld]>}{}gs;
    $info =~ s{(</?)dt(>)}{$1."screen".$2}gsex;

    $info =~ s{(</?)code(>)}{$1."literal".$2}gsex;
    $info =~ s{(</?)p\b([^<>]*>)}{$1."para".$2}gsex;

    return "<varlistentry><term>description</term>"
	."<listitem>".$info."</listitem></varlistentry>";
}

$T =~ s{<pfewordsetinfo[^<>]*>
	    ((?:.(?!</?pfewordsetinfo\b))*.) </pfewordsetinfo>}
{ &pfewordsetinfo($1) }gsex;

# ---------------------------------------------------------------------
my $n;

if ($opt{dequote})
{
    for $n (keys %V) { $V{$n} =~ s:\&quot\;:\&\#x0022\;:; }
}

my %missing = ( "ENVIRONMENT" => "wordset::environ",
		"POCKET" => "hold:core:",
		"EDITOR" => "wordset::edit",
		"ONLY"   => "wordset::search",
		"LOADED" => "wordset::search");
my %aliases = ( "VAR" => "LVALUE", "NEST" => ":",
		"TM_STOP_OFF" => "TM_RUN_HERE",
		"TS_RUN_INIT" => "TM_RUN_HERE",
		"Z+PLACE" => "+ZPLACE",
		"TM_IPC_SENDME" => "TM_IPC_SEND",
		"_accept_" => "ACCEPT",
		"SP" => "SP\@", "CFA" => "&gt;BODY",
		"CS-STACK" => "CS-SWAP", "OFFSET-RT" => "+CONSTANT",
		"HIDE" => "REVEAL", "/DCELL" => "/CELL", "DCELLS" => "CELLS",
		"/TIB" => "TIB", "&gt;EXECUTE" => "EXECUTE",
		"&lt;A&gt;" => "LOCALS|", "L\@" => "\@",
		"&lt;B&gt;" => "LOCALS|", "L\!" => "\!",
		"&lt;N&gt;" => "LOCALS|", "*\\/" => "*\/" );

for $n (keys %missing)
{
    next if exists $link{$n};
    $link{$n}{"(missing)"} = $missing{$n};
}
for $n (keys %aliases)
{
    next if exists $link{$n};
    my $K = scalar each %{$link{ $aliases{$n} }};
    $K = scalar each %{$link{ $aliases{$n} }} if not defined $K;
    next if not length $K;
    $link{$n}{"(aliases)"} = $link{ $aliases{$n} }{$K};
}

for $n (keys %V)
{
    my $wordset = "."; if ($V{$n} =~ m{id=\"[^:\"]+([\w:]+)\"}) {$wordset=$1;}

    $V{$n} =~ s{(<link>)([^<>]+)(</link>)}
    { my $out = $1; if (exists $link{$2}) {
	if (exists $link{$2}{$wordset}) { # print STDERR "#";
	    $out = '<link linkend="'.$link{$2}{$wordset}.'">';
	}else{
	    my $K = scalar each %{$link{$2}};
	    $K = scalar each %{$link{$2}} if not defined $K;
	    $out = '<link linkend="'.$link{$2}{$K}.'">' if length $K;
	}
    }; $out.$2.$3 }gsex;
    $V{$n} =~ s{<link(>[\(]+) ([^\<\>\(\)]+) ([\)]+</link>)}
    { my $out = "<link"; if (exists $link{$2}) {
	if (exists $link{$2}{$wordset}) { # print STDERR "#";
	    $out = '<link linkend="'.$link{$2}{$wordset}.'"';
	}else{
	    my $K = scalar each %{$link{$2}};
	    $K = scalar each %{$link{$2}} if not defined $K;
	    $out = '<link linkend="'.$link{$2}{$K}.'"' if length $K;
	}
    }; $out.$1.$2.$3 }gsex;
    $V{$n} =~ s{(<link>)([^<>]+)(</link>)}
    {<link linkend=\"reference\">$2$3}gs;
}

### if ($opt{nodoctype})
{ # the $T text as well
    my $wordset = ":core";
    $T =~ s{(<link>)([^<>]+)(</link>)}
    { my $out = $1; if (exists $link{$2}) {
	if (exists $link{$2}{$wordset}) { # print STDERR "#";
	    $out = '<link linkend="'.$link{$2}{$wordset}.'">';
	}else{
	    my $K = scalar each %{$link{$2}};
	    $K = scalar each %{$link{$2}} if not defined $K;
	    $out = '<link linkend="'.$link{$2}{$K}.'">' if length $K;
	}
    }; $out.$2.$3 }gsex;
    $T =~ s{<link(>[\(]+) ([^\<\>\(\)]+) ([\)]+</link>)}
    { my $out = "<link"; if (exists $link{$2}) {
	if (exists $link{$2}{$wordset}) { # print STDERR "#";
	    $out = '<link linkend="'.$link{$2}{$wordset}.'"';
	}else{
	    my $K = scalar each %{$link{$2}};
	    $K = scalar each %{$link{$2}} if not defined $K;
	    $out = '<link linkend="'.$link{$2}{$K}.'"' if length $K;
	}
    }; $out.$1.$2.$3 }gsex;
    $T =~ s{(<link>)([^<>]+)(</link>)}
    {<link  linkend=\"reference\">$2$3}gs;
}

# ---------------------------------------------------------------------
print STDERR "\n";

my $title = "";
$title .= ($opt{title} ? $opt{title} : "Words Reference");

$opt{reference} = 1 if not $opt{reference} and not $opt{wordsets};

$n = "\n";
if ($opt{wordsets})
{
    print
	'<!DOCTYPE chapter PUBLIC "-//OASIS//DTD DocBook XML V4.1.2//EN"',$n,
	'   "http://www.oasis-open.org/docbook/xml/4.1.2/docbookx.dtd">',$n,$n
	unless $opt{nodoctype};
    print $T;
}

if ($opt{reference})
{
    print
       '<!DOCTYPE reference PUBLIC "-//OASIS//DTD DocBook XML V4.1.2//EN"',$n,
       '   "http://www.oasis-open.org/docbook/xml/4.1.2/docbookx.dtd">',$n,$n
	unless $opt{nodoctype};
    print "<reference id=\"reference\"><title>$title</title>",
    "  <!-- -*- xml -*- -->",$n;
    for $T (sort keys %V) { print $V{$T},$n; }
    print "</reference>",$n;
}

if ($opt{missing})
{
    print STDERR "missing notations: ";
    for $i (sort keys %nostacknotation)
    {
	print STDERR $i," ( [",$nostacknotation{$i},"] ) - ";
    }
    print STDERR $n;
}

exit 0;

