AUTOMAKE_OPTIONS = 1.6 foreign no-dist-gzip dist-bzip2 dist-lzma
AUTOTOOL_VERSION = autoconf-2.52 automake-1.5 libtool-1.4.2
ACLOCAL_AMFLAGS = -I ../../../config/m4

SUBDIRS = include/pfe c doc

pkgconfigdir = ${libdir}/pkgconfig
pkgconfig_DATA = pfe.pc

auto:
	aclocal
	autoconf
	autoheader
	automake -a -c

config:
	rm -f config.cache
	@configurecall@

reconfigure:
	@configurecall@

install-data-local : install-scripts install-transform-pkgconfig
install-exec-hook :
	for bin in pfe-exec pfe ; do (cd $(DESTDIR)$(bindir) && \
         ln -sf `echo $$bin | sed '$(transform)'` $$bin); done

SCRIPTDIR= $(srcdir)/..
install-scripts : 
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/test
	$(INSTALL_DATA) $(SCRIPTDIR)/test/*.fs  $(DESTDIR)$(pkgdatadir)/test
	$(INSTALL_DATA) $(SCRIPTDIR)/test/*.4th $(DESTDIR)$(pkgdatadir)/test
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/lib
	$(INSTALL_DATA) $(SCRIPTDIR)/share/*.4th $(DESTDIR)$(pkgdatadir)/lib
install-transform-pkgconfig:
	$(mkinstalldirs) $(DESTDIR)$(pkgconfigdir)
	@ for f in $(pkgconfig_DATA) ; do test -f $$f || f="$(srcdir)/$$f" \
	; ff=`echo $$f | sed -e '$(transform)'` ; if test "$$f" != "$$ff" \
	; then sed -e "/Name:/$(transform)" $$f > $$ff \
	; if test -s $ff ; then if diff $$f $$ff >/dev/null 2>/dev/null \
	; then echo $(INSTALL_SCRIPT) $$ff $(DESTDIR)$(pkgconfigdir) \
	; $(INSTALL_SCRIPT) $$ff $(DESTDIR)$(pkgconfigdir) ; fi fi fi done
install-it installs : install

it : all 

# config.h : incl-gen.t

TOOL_PFE_DIR = $(srcdir)/../../tool/pfe

incl-gen : incl-gen.t
incl-gen.t : # make sure it is built atleast after unpacking
	$(MAKE) $(srcdir)/incl-gen.h
	cp $(srcdir)/incl-gen.h $@

      # if perl is not installed, the old files are left untouched.
$(srcdir)/incl-gen.h : $(pkginc_gen_headers:.h=.c)
	: perl $(TOOL_PFE_DIR)/Make-H.pl $(srcdir)/...
	@ for F in $? "" ; do if test ! -z "$$F" \
	; then   perl $(TOOL_PFE_DIR)/Make-H.pl $$F \
	; fi done ; true
	@ echo creating $@ "(master include)" \
	; echo '/*' `date` '*/' >$@ \
	; for F in $(pkginc_gen_headers) ; do : \
	; echo '#include <pfe/'$$F'>' >>$@ \
	; done
gen : $(srcdir)/incl-gen.h
force-gen:
	: perl $(TOOL_PFE_DIR)/Make-H.pl $(srcdir)/...
	@ for F in $(pkginc_gen_headers:.h=.c) "" ; do if test ! -z "$$F" \
	; then   perl $(TOOL_PFE_DIR)/Make-H.pl $(srcdir)/$$F \
	; fi done ; true

distclean-genheaders:
	cd $(srcdir) && rm -f $(pkginc_gen_headers)

