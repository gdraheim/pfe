AUTOMAKE_OPTIONS = 1.5 foreign dist-bzip2
AUTOTOOL_VERSION = autoconf-2.52 automake-1.5 libtool-1.4.2
ACLOCAL_AMFLAGS = -I uses
# $Id: Makefile.am,v 1.12 2008-12-22 09:37:21 guidod Exp $

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
pdfs mans install-mans man1 install-man1 man3 install-man3 \
changes.htm forthxml forthxmls clean-forthxml clean-forthxmls:
	(cd doc && $(MAKE) $@)

incl-gen.t incl-gen.h force-gen gen :
	(cd pfe && $(MAKE) $@)
testlist tests checks anstests:
	(cd test && $(MAKE) $@)


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

rpm : dist-bzip2
	rpmbuild -ta $(PACKAGE)-$(VERSION).tar.bz2
	test -d $(top_srcdir)/pub || mkdir $(top_srcdir)/pub 
	cp $(PACKAGE)-$(VERSION).tar.bz2 $(top_srcdir)/pub/

OSC_ROOT = /my/own/osc
OSC_SRCDIR = $(OSC_ROOT)
OSC_PROJECT = home:guidod:devel
OSC_PACKAGE = $(PACKAGE)
OSC_SPECFILE = $(top_srcdir)/$(PACKAGE).spec
OSC_TARBALL = $(PACKAGE)-$(VERSION).tar.bz2
OSC_BUILDDIR = $(OSC_ROOT)
OSC_PACKAGECACHEDIR = $(OSC_BUILDDIR)/packagecache
OSC_BUILDROOT = $(OSC_BUILDDIR)/buildroot
OSC_REPOSITORY = openSUSE_11.2
OSC_ARCH = x86_64
osc-copy:
	test -s $(OSC_TARBALL)
	test -s $(OSC_SPECFILE)
	test -d $(OSC_SRCDIR)
	test -d $(OSC_SRCDIR)/$(OSC_PROJECT) \
	|| ( cd $(OSC_SRCDIR) && osc co $(OSC_PROJECT) )
	cd $(OSC_SRCDIR)/$(OSC_PROJECT) && osc up
	cp $(OSC_TARBALL)  $(OSC_SRCDIR)/$(OSC_PROJECT)/$(OSC_PACKAGE)
	cp $(OSC_SPECFILE) $(OSC_SRCDIR)/$(OSC_PROJECT)/$(OSC_PACKAGE)

DOIT = :
osc-ci osc-commit upload-osc: osc-copy
	tarball=`basename $(OSC_TARBALL)` \
	; ls  $(OSC_SRCDIR)/$(OSC_PROJECT)/$(OSC_PACKAGE)/*.tar.* \
	| { while read f; do tarname=`basename $$f` \
	; if test "$$tarname" != "$$tarball" \
	; then echo osc rm $$f ; osc rm $$f \
	; else echo osc add $$f ; osc add $$f \
	; fi ; done }
	osc status $(OSC_SRCDIR)/$(OSC_PROJECT)/$(OSC_PACKAGE)/
	$(DOIT) osc commit -m "update $(OSC_TARBALL)" $(OSC_SRCDIR)/$(OSC_PROJECT)/$(OSC_PACKAGE)

osc-build: osc-copy
	cd $(OSC_SRCDIR)/$(OSC_PROJECT)/$(OSC_PACKAGE)/ \
	BUILD_DESCR=`basename $(OSC_SPECFILE)` ;\
	ls -l $$BUILD_DESCR ; \
	OSC_PACKAGECACHEDIR=$(OSC_PACKAGECACHEDIR) \
	OSC_BUILD_ROOT=$(OSC_BUILDROOT) \
	osc build --release=0 --local-package --clean \
	$(OSC_REPOSITORY) $(OSC_ARCH) $$BUILD_DESCR
osc-clean:
	- test -f $(OSC_SRCDIR)/$(OSC_PROJECT)/$(OSC_PACKAGE)/*.spec \
	&& rm $(OSC_SRCDIR)/$(OSC_PROJECT)/$(OSC_PACKAGE)/*
osc-distclean:
	- test -f $(OSC_SRCDIR)/$(OSC_PROJECT)/$(OSC_PACKAGE)/*.spec \
	&& rm -rf $(OSC_SRCDIR)/*
		
# --------------------------------------------------------------------
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
