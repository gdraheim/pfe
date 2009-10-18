srcdir = ../../testmodule2
top_srcdir = ../../testmodule2
top_builddir = .
VPATH = ../../testmodule2
PKG_CONFIG= /bin/sh ../src/lib/pfe/pfe-uninstalled.sh
LIBTOOL = $(SHELL) $(top_builddir)/libtool
CC = gcc
CCLD = $(CC)

LIBS   =  -L/home/guidod/own/workspace/pfe-34/build/src/lib/pfe/.libs -W,--warn-common -lpfe -ldl
CFLAGS = -g -O2 -I/home/guidod/own/workspace/pfe-34/build/src/lib/pfe/../../../../src/lib/pfe  -I..
CPPFLAGS = 
DEFS = -DPACKAGE_NAME=\"testmodule2-ext.c\" -DPACKAGE_TARNAME=\"testmodule2-ext-c\" -DPACKAGE_VERSION=\"testmodule2\" -DPACKAGE_STRING=\"testmodule2-ext.c\ testmodule2\" -DPACKAGE_BUGREPORT=\"0.2.0\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DLT_OBJDIR=\".libs/\"
LDFLAGS = 
pfeversion = 0.34.01
pfeincludedir = /home/guidod/own/workspace/pfe-34/build/src/lib/pfe/../../../../src/lib/pfe
pfedatadir = /usr/local/share/pfe
pfelibdir = /home/guidod/own/workspace/pfe-34/build/src/lib/pfe/.libs/pfe
pfesuffix = -forth
pfepluginsdir = ${pfelibdir}${pfesuffix}
DISTFILES = $(testmodule2_la_SOURCES) testmodule2.fs \
            Makefile.in configure.ac configure aclocal.m4 COPYING.ZLIB 
#           ltmain.sh mkinstalldirs depcomp install-sh config.guess config.sub

all : testmodule2.la

LINK =      $(LIBTOOL) --mode=link $(CCLD) $(CFLAGS) $(LDFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) \
        $(DEFAULT_INCLUDES) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)

testmodule2_la_SOURCES = testmodule2-ext.c testmodule2-dll.c
testmodule2.la : $(testmodule2_la_SOURCES:.c=.lo)
	$(LINK) -rpath $(pfepluginsdir) -module -avoid-version $(LIBS) \
           -o $@ $(testmodule2_la_SOURCES:.c=.lo)
testmodule2-dll.c : $(pfeincludedir)/pfe/module-dll.c
	sed -e s/module/testmodule2/ "$?" > $@

install : install-plugins install-data
install-plugins:
	- mkdir -p "$(DESTDIR)$(pfepluginsdir)"
	for f in testmodule2.la ; do test -f "$$f" || f="$(srcdir)/$$f" \
	; $(LIBTOOL) --mode=install cp "$$f" "$(DESTDIR)$(pfepluginsdir)/" \
        || exit 1 ; done
install-data:
	- mkdir -p "$(DESTDIR)$(pfedatadir)"
	for f in testmodule2.fs ; do test -f "$$f" || f="$(srcdir)/$$f" \
	; $(LIBTOOL) --mode=install cp "$$f" "$(DESTDIR)$(pfedatadir)/" \
        || exit 1 ; done

.SUFFIXES: .c .lo .o .obj .la .pc
.c.lo : 
	$(LTCOMPILE) $< -c -o $@

distdir : 
	test -d $(distdir) || mkdir $(distdir)
	@ srcdir="$(srcdir)" ; test ".$srcdir" != "." || srcdir="." \
	; for file in $(DISTFILES) ; do echo cp $$srcdir/$$file \
	; cp -p $$srcdir/$$file $(distdir) || exit 1 ; done \
	; chmod a+rw $(distdir)/*

clean :
	- rm *~ *.o *.so *.lo*
distclean : clean
	- rm -r libtool Makefile config.log config.status autom4te.cache
check distcheck uninstall :
