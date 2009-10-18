#! /usr/bin/perl

# convert a block (forth) script into a serial (forth) script
# a block script has a fixed linelength of 64 chars and no
# line end character.

for (@ARGV)
{
    if (/^--(.*)/)
    {
	print "no options available, sorry";
    }else{
	my $F = $_;
	my $line;
	open F, "<$F" or next;
	while ((read F, $line, 64))
	{
	    print $line,"\n";
	}
	close F;
    }
}
