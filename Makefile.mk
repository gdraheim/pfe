PUB=pub/

autoreconf:
	autoreconf -i -f
	cd libpfe && autoconf -i -f
	cd exepfe && autoreconf -i -f
	cd testmodule1 && autoreconf -i -f
	cd testmodule2 && autoreconf -i -f
	cd pfe_stackhelp && autoreconf -i -f
	cd pfe_experimental && autoreconf -i -f
	test -f config/m4/ltdl.m4 || \
	cp /usr/share/aclocal/ltdl.m4 config/m4/ltdl.m4

auto:
	aclocal -I tool/m4 && autoconf && automake
	(cd libpfe/src/main && aclocal -I ../../../config/m4 && autoconf && automake)
	(cd libpfe/src/main && autoheader) || true
	(cd exepfe && aclocal -I ../config/m4 && autoconf && automake)
	(cd testmodule1 && aclocal -I ../config/m4 && autoconf && automake)
	(cd testmodule2 && aclocal -I ../config/m4 && autoconf)
	(cd libpfe/src/main && libtoolize --force --copy)
	(cd testmodule1 && libtoolize --force --copy)
	(cd testmodule2 && libtoolize --force --copy)
	test -f config/m4/ltdl.m4 || \
	cp /usr/share/aclocal/ltdl.m4 config/m4/ltdl.m4

rebuild: auto
	- rm Makefile
	bash -c 'for variant in regs fig forth call calls fast . \
	; do bash configure --prefix=$$HOME/$$OSTYPE \
	  --with-variant=$$variant \
	; done'
	$(MAKE) -f Makefile
	$(MAKE) -f Makefile check
	echo "#       make install-all"

make-rw:
	chmod -R +w .

# ----------------------------------------------------------------

RPM_SOURCEDIR= $(shell rpmbuild -E '%_sourcedir' 2>/dev/null)
RPM_SPECDIR= $(shell rpmbuild -E '%_sourcedir' 2>/dev/null)

rpms:
	$(MAKE) rpm-variant variant=forth 'args='
	$(MAKE) rpm-variant variant=call 'args=--with call'
	: $(MAKE) rpm-variant variant=calls 'args=--with call --with sbr'
	: $(MAKE) rpm-variant variant=fast 'args=--with sbr'
	: $(MAKE) rpm-variant variant=fastest 'args=--with sbr --with regs'

rpm-variant:
	- cp $(PUB)/$(PACKAGE)-$(VERSION).tar.bz2 $(RPM_SOURCEDIR)
	sed -e '/define variant/s|forth|$(variant)|' pfe.spec \
            > $(RPM_SPECDIR)/pfe-$(variant).spec
	rpmbuild --rmsource --rmspec $(args) \
          -ba $(RPM_SPECDIR)/pfe-$(variant).spec

rpm: dist-bzip $(PACKAGE).spec
	rpmbuild -ta $(PUB)$(PACKAGE)-$(VERSION).tar.bz2

dist-bzip : dist-bzip2
	$(MAKE) dist-bzip2-done
dist-bzip2-done dist-done :
	test -d $(PUB) || mkdir $(PUB)
	@ echo cp $(BUILD)/$(PACKAGE)-$(VERSION).tar.bz2 $(PUB). \
	;      cp $(BUILD)/$(PACKAGE)-$(VERSION).tar.bz2 $(PUB).
snapshot:
	$(MAKE) dist-bzip2 VERSION=`date +%Y.%m%d`
distclean-done:
	- rm -r *.d

configsub :
	cp ../savannah.config/config.guess config/aux/config.guess
	cp ../savannah.config/config.sub   config/aux/config.sub
