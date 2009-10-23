AUTOMAKE_OPTIONS = 1.5 foreign dist-bzip2
AUTOTOOL_VERSION = autoconf-2.52 automake-1.5 libtool-1.4.2
ACLOCAL_AMFLAGS = -I ../config/m4

LIBS   = @PFE_LIBS@ @LIBS@
CFLAGS = @PFE_CFLAGS@ @CFLAGS@ -I..
pfelibdir = @PFE_LIBDIR@
pfesuffix = @PFE_SUFFIX@
pfepluginsdir = ${pfelibdir}${pfesuffix}
pfeplugins_LTLIBRARIES = socket.la

socket_la_SOURCES = \
    src/main/c/socket-dll.c \
    src/main/c/socket-ext.c 

socket_la_LDFLAGS = -export-dynamic -module -avoid-version

src/main/c/socket-dll.c : @pfeincludedir@/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed -e s/module/socket/ "$?" >> $@
