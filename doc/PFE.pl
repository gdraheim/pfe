package PFE;
use xm::pp;
use strict;

my $tekborder = 
q(
  <!-- Tek Header -->
  <table border="0" cellpadding="0" cellspacing="0" width="690">
  <tr>
    <td width="651" valign="top" colspan="2" align="left"><h2 align="right"><img
    src="/tek_bln.gif" alt="TekWeb" width="640" height="75"><br>
    Entwicklung</h2>
    </td>
  </tr>
  <tr height="13">
    <td valign="top" width="118">
  <p>
  <a href="http://intranet.berl.tek.com" alt="intranet link">Intranet/</a>
  <br />
  <a href="http://intranet.berl.tek.com/entwicklung"
    alt="intranet development">Entwicklung/</a>
  <br />
  <a href="http://intranet.berl.tek.com/entwicklung/forth"
    alt="intranet forth">Forth/</a>
  </p>
  </td><td>

  <!-- / Tek Header -->
  ${*}
  <!-- Tek Footer -->
  </td></tr></table>

    <table> 
    <tr><td colspan=2>
    <img src="/hori_line4.gif" alt="-----------------------------------------">
    </td></tr>
    <tr><td align=left><i><small><small>
    <a href="mailto:Guido.Draheim@exgate.tek.com"> Guido Draheim </a>
    , modified $Date: 2000-09-25 14:30:25 $</small></small></i>
    <td align=right> <a href=".."><small>back</small></a>
    </table>
);

my $sfgborder = 
q(
    <table><tr><td align=center>
    <h1> Portable Forth Environment </h1>
    </td><td> SourceForge <br /> Logo (FIXME:) 
    </td></tr></table>
    <hr /><p>
);

sub sitebody
{
    my $uname_n = `uname -n`;
    my $body;
    if ($uname =~ /sun\d+/ or $uname =~ /swe\d+/)
    {
	$body = $tekborder;
    }else{
	$body = $sfgborder;
    }

    if ($body =~ s{ \$\{\*\} } { $_[0] }sex)
    {
	return $body;
    }else{
	return $body.$_[0];
    }
}
xm::pp::call2 "sitebody", \&sitebody;
