AUTOMAKE_OPTIONS = 1.5 foreign dist-bzip2
AUTOTOOL_VERSION = autoconf-2.52 automake-1.5 libtool-1.4.2
ACLOCAL_AMFLAGS = -I uses
# $Id: Makefile.am,v 1.7 2006-09-26 18:06:05 guidod Exp $

ACSUBDIRS    = pfe bin testmodule1 testmodule2
SUBDIRS      = pfe bin lib test doc @EXTRAMODULES@
DIST_SUBDIRS = pfe bin lib test doc testmodule1 testmodule2

dbk_MAKEFILE = dbk/pfe-fth.dbk dbk/pfe-fth.mk dbk/wordsets2words.pl
mk_MAKEFILE = mk/fb-2-fs.pl  mk/Make-H.pl \
              mk/fb-2-fs.py  mk/Make-H.py
EXTRA_DIST = autogen.sh Makefile.mk pfe.spec INSTALL.TXT \
        $(mk_MAKEFILE) $(dbk_MAKEFILE)

# =======================================================================
# `make` will only make bin/libs/man1 but none of man3/docs
# Now use `make it` to `make mans` which ensures that (a) xmlto/xsl
# is available and (b) the generated docbook master is correct.
it : all mans

doc  docs install-doc install-docs docu docus htmfiles install-sf \
pdfs mans install-mans man1 install-man1 man3 install-man3 :
	(cd doc && $(MAKE) $@)

incl-gen.t incl-gen.h force-gen gen :
	(cd pfe && $(MAKE) $@)

pfe-exec :
	(cd bin && $(MAKE) $@)

testmodule.la :
	(cd lib && $(MAKE) $@)

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

gdb-ld:
	@ echo run --lib-path-string=pfe/.libs --inc-path-string=$(srcdir)
	LD_LIBRARY_PATH=pfe/.libs gdb pfe/.libs/pfe
gdb-dyld:
	@ echo run --lib-path-string=pfe/.libs --inc-path-string=$(srcdir)
	DYLD_LIBRARY_PATH=pfe/.libs gdb pfe/.libs/pfe
gdb :
	@ echo make gdb-ld
	@ echo make gdb-dyld
run:
	pfe/pfe --lib-path-string=pfe/.libs
