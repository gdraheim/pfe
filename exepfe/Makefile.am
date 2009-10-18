AUTOMAKE_OPTIONS = 1.5 foreign dist-bzip2
AUTOTOOL_VERSION = autoconf-2.52 automake-1.5 libtool-1.4.2
ACLOCAL_AMFLAGS = -I ../config/m4
# $Id: Makefile.am,v 1.2 2006-08-11 22:56:03 guidod Exp $

LIBPFE_MAIN = ../libpfe/src/main
LIBPFE_INCLUDES = $(LIBPFE_MAIN)/include

# do not do -I$(srcdir) !!
DEFS= @DEFS@ -I. -I$(LIBPFE_INCLUDES) -I$(srcdir)/$(LIBPFE_INCLUDES) 

EXEPFE_MAIN = src/main
EXEPFE_DIR = $(EXEPFE_MAIN)/c

bin_PROGRAMS = pfe-exec
pfe_exec_SOURCES = $(EXEPFE_DIR)/pfe-exec.c

LIBPFE_DIR = $(LIBPFE_MAIN)/c
pfe_exec_LDADD = $(LIBPFE_DIR)/libpfe.la 

$(LIBPFE_DIR)/libpfe.la : $(srcdir)/$(LIBPFE_DIR)/*.c
	(cd $(LIBPFE_DIR) && $(MAKE) `basename $@`)

