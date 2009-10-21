AUTOMAKE_OPTIONS = 1.5 foreign dist-bzip2
AUTOTOOL_VERSION = autoconf-2.52 automake-1.5 libtool-1.4.2
ACLOCAL_AMFLAGS = -I ../config/m4

LIBS   = @PFE_LIBS@ @LIBS@
CFLAGS = @PFE_CFLAGS@ @CFLAGS@ -I..
pfelibdir = @PFE_LIBDIR@
pfesuffix = @PFE_SUFFIX@
pfepluginsdir = ${pfelibdir}${pfesuffix}
pfeplugins_LTLIBRARIES = stackhelp.la

stackhelp_la_SOURCES = \
    src/main/c/stackhelp-dll.c \
    src/main/c/stackhelp-ext.c \
    src/main/c/core-stk.c \ 
    src/main/c/block-stk.c \
    src/main/c/cdecl-stk.c \
    src/main/c/chain-stk.c \
    src/main/c/chainlist-stk.c \
    src/main/c/file-stk.c \
    src/main/c/memory-alloc-stk.c \ 
    src/main/c/string-stk.c \
    src/main/c/stackhelp-stk.c \
    src/main/c/exception-stk.c \ 
    src/main/c/facility-stk.c \
    src/main/c/misc-stk.c \
    src/main/c/debug-stk.c \
    src/main/c/forth-83-stk.c 
    
stackhelp_la_LDFLAGS = -export-dynamic -module -avoid-version

src/main/c/stackhelp-dll.c : @pfeincludedir@/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed -e s/module/stackhelp/ "$?" >> $@
