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
it: all
