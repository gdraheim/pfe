AUTOMAKE_OPTIONS = 1.5 foreign dist-bzip2
AUTOTOOL_VERSION = autoconf-2.52 automake-1.5 libtool-1.4.2
ACLOCAL_AMFLAGS = -I ../uses
# $Id: Makefile.am,v 1.2 2006-08-11 22:56:03 guidod Exp $

DEFS= @DEFS@ -I. -I.. -I$(srcdir)/..            # do not do -I$(srcdir) !!

bin_PROGRAMS = pfe-exec

pfe_exec_LDADD = ../pfe/libpfe.la 

../pfe/libpfe.la : $(srcdir)/../pfe/*.c
	(cd ../pfe && $(MAKE) `basename $@`)

