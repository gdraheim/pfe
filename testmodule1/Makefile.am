AUTOMAKE_OPTIONS = 1.5 foreign dist-bzip2
AUTOTOOL_VERSION = autoconf-2.52 automake-1.5 libtool-1.4.2
ACLOCAL_AMFLAGS = -I ../uses

pfedatadir = @pfedatadir@
pfedata_DATA = testmodule1.fs
EXTRA_DIST = $(DATA) COPYING.ZLIB

LIBS   = @PFE_LIBS@ @LIBS@
CFLAGS = @PFE_CFLAGS@ @CFLAGS@ -I..
pfelibdir = @PFE_LIBDIR@
pfesuffix = @PFE_SUFFIX@
pfepluginsdir = ${pfelibdir}${pfesuffix}
pfeplugins_LTLIBRARIES = testmodule1.la

testmodule1_la_SOURCES = testmodule1-ext.c testmodule1-dll.c
testmodule1_la_LDFLAGS = -module -avoid-version

testmodule1-dll.c : @pfeincludedir@/pfe/module-dll.c
	sed -e s/module/testmodule1/ "$^" > $@

