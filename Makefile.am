AUTOMAKE_OPTIONS = 1.6 foreign no-dist-gzip dist-bzip2 dist-lzma
AUTOTOOL_VERSION = autoconf-2.52 automake-1.5 libtool-1.4.2
ACLOCAL_AMFLAGS = -I config/m4
# $Id: Makefile.am,v 1.12 2008-12-22 09:37:21 guidod Exp $

ACSUBDIRS    = libpfe exepfe testmodule1 testmodule2 pfe_stackhelp
SUBDIRS      = libpfe exepfe @EXTRAMODULES@
DIST_SUBDIRS = libpfe exepfe testmodule1 testmodule2 pfe_stackhelp

EXTRA_DIST = autogen.sh Makefile.mk pfe.spec 

# =======================================================================
# `make` will only make bin/libs/man1 but none of man3/docs
# Now use `make it` to `make mans` which ensures that (a) xmlto/xsl
# is available and (b) the generated docbook master is correct.
it : all mans

doc  docs install-doc install-docs docu docus htmfiles install-sf \
pdfs mans install-mans man1 install-man1 man3 install-man3 \
changes.htm forthxml forthxmls clean-forthxml clean-forthxmls:
	(cd libpfe/src/main/doc && $(MAKE) $@)

incl-gen.t incl-gen.h force-gen gen headers gendeps :
	(cd libpfe/src/main/include/pfe && $(MAKE) $@)
testlist tests checks anstests:
	(cd libpfe/src/test && $(MAKE) $@)


pfe-exec :
	(cd exepfe && $(MAKE) $@)

testmodule.la :
	(cd libpfe/src/share && $(MAKE) $@)

distt-bzip2 : dist-bzip2
	test "$top_builddir" = "$top_srcdir" || \
	cp $(PACKAGE)-$(VERSION).tar.bz2 $(top_srcdir)

distt : dist
	test "$top_builddir" = "$top_srcdir" || \
	cp $(PACKAGE)-$(VERSION).tar.gz $(top_srcdir)

zipp : distdir
	@ test "$(distdir)/" != "/"
	for i in $(distdir)/*/* ; do test -d $$i && continue \
	; if head $$i | grep generated >/dev/null \
	; then echo rm $$i ; rm $$i ; fi ; done
	- rm doc/*.ar doc/*.pdf
	zip -m9r $(distdir).ZIP $(distdir)/*
	test "$top_builddir" = "$top_srcdir" || \
	cp $(distdir).ZIP $(top_srcdir)

rpm : dist-bzip2
	rpmbuild -ta $(PACKAGE)-$(VERSION).tar.bz2
	test -d $(top_srcdir)/pub || mkdir $(top_srcdir)/pub 
	cp $(PACKAGE)-$(VERSION).tar.bz2 $(top_srcdir)/pub/

gdb-ld:
	@ echo run --lib-path-string=pfe/.libs --inc-path-string=$(srcdir)
	LD_LIBRARY_PATH=libpfe/src/main/c/.libs gdb libpfe/src/main/c/.libs/pfe
gdb-dyld:
	@ echo run --lib-path-string=pfe/.libs --inc-path-string=$(srcdir)
	DYLD_LIBRARY_PATH=libpfe/src/main/c/.libs gdb libpfe/src/main/c/.libs/pfe
gdb :
	@ echo make gdb-ld
	@ echo make gdb-dyld
run:
	libpfe/src/main/c/pfe --lib-path-string=libpfe/src/main/c/.libs
