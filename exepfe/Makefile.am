AUTOMAKE_OPTIONS = 1.5 foreign dist-bzip2
AUTOTOOL_VERSION = autoconf-2.52 automake-1.5 libtool-1.4.2
ACLOCAL_AMFLAGS = -I ../config/m4
# $Id: Makefile.am,v 1.2 2006-08-11 22:56:03 guidod Exp $

# do not do -I$(srcdir) !!
DEFS= @DEFS@ -I. -I../libpfe/src/include -I$(srcdir)/../libpfe/src/include 

bin_PROGRAMS = pfe-exec

pfe_exec_LDADD = ../pfe/libpfe.la 

../src/lib/pfe/libpfe.la : $(srcdir)/../src/lib/pfe/*.c
	(cd ../src/lib/pfe && $(MAKE) `basename $@`)

