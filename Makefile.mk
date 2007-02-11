PUB=pub/

autoreconf:
	autoreconf -i -f
	cd testmodule1 && autoreconf -i -f
	cd testmodule2 && autoreconf -i -f

auto:
	aclocal -I uses && autoconf && automake
	(cd pfe && aclocal -I ../uses && autoconf && automake)
	(cd pfe && autoheader) || true
	(cd bin && aclocal -I ../uses && autoconf && automake)
	(cd testmodule1 && aclocal -I ../uses && autoconf && automake)
	(cd testmodule2 && aclocal -I ../uses && autoconf)
	(cd pfe         && libtoolize --force --copy)
	(cd testmodule1 && libtoolize --force --copy)
	(cd testmodule2 && libtoolize --force --copy)

sweep:
	- grep -l "Make-H" pfe/*.h | xargs rm
	- rm doc/changes.htm doc/pfe.1 doc/pfe.htm
	- rm pfe/incl-gen.h
	- for i in pfe/*-dll.c \
	; do test ".$$i" = ".pfe/module-dll.c" && continue  \
	; echo rm $$i ; rm $$i ; done

# NOTE: Tek cms directories contain "~" which breaks libtool
sun: auto
	- rm Makefile
	bash -c 'bash configure --prefix=$$HOME/$$OSTYPE'
	- cd Release/sparc*/pfe ; sed -e "s,IFS='~',IFS='?'," \
          -e "/^[a-z_]*_cmds=/s,~,?,g" libtool >libtool.new \
	; test -s libtool.new && mv libtool.new libtool
	make-sun # $(MAKE) -f Makefile
	make check && make install

tek: auto
	- rm Makefile
	bash -c 'bash configure --prefix=$$HOME/$$OSTYPE'
	$(MAKE) -f Makefile
	echo "#       make check && make install"

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
	cp ../savannah.config/config.guess uses/config.guess
	cp ../savannah.config/config.sub   uses/config.sub

cf = $(cf_fedora)
cf_debian = x86-linux1
cf_fedora = x86-linux2
cf_freebsd = x86-freebsd1
cf_netbsd = x86-netbsd1
cf_openbsd = x86-openbsd1
cf_amd64 = amd64-linux1
cf_alpha = usf-cf-alpha-linux-1
cf_powermac = ppc-osx3
cf_powerpc = openpower-linux1
cf_sparc = sparc-solaris1
cf_solaris = x86-solaris1
linux = $(cf_fedora)  $(cf_debian)   $(cf_amd64)   $(cf_powerpc)   $(cf_alpha) 
bsd =   $(cf_freebsd) $(cf_netbsd)   $(cf_openbsd) $(cf_powermac)
sun =   $(cf_sparc)   $(cf_solaris)
all = $(linux) $(sun) $(bsd)
list = all


_paths_ = export PATH=$$PATH:X/bin
_includes_ = CPPFLAGS=\"-IX/include -IX/include/ncurses\"
sparc-solaris1-gcc-path = $(subst X,/usr/local,$(_paths_));
x86-solaris1-gcc-path   = $(subst X,/usr/local,$(_paths_));
sparc-solaris1-cc-path  = $(subst X,/opt/SUNWspro,$(_paths_));
x86-solaris1-cc-path    = $(subst X,/opt/SUNWspro,$(_paths_));
sparc-solaris1-gcc-conf = $(subst X,/usr/local,$(_includes_));
x86-solaris1-gcc-conf   = $(subst X,/usr/local,$(_includes_));
sparc-solaris1-cc-conf  = $(subst X,/opt/SUNWspro,$(_includes_));
x86-solaris1-cc-conf    = $(subst X,/opt/SUNWspro,$(_includes_));
cf_compiler=gcc
cf_configure =    sh configure $($(cf)-$(cf_compiler)-conf) $(args)
cf_profile = $(cf_get_uname); $($(cf)-$(cf_compiler)-path)
cf_get_uname = uname -msr
cf_unpacked = $(PACKAGE)-$(VERSION)
cf : cf-upload cf-system-all
cf-upload: ;	scp $(PUB)$(PACKAGE)-$(VERSION).tar.bz2 cf-shell.sf.net:
cf-unpack: ; ssh cf-shell.sf.net \
	ssh $(cf) "'tar xjvf $(PACKAGE)-$(VERSION).tar.bz2 2>&1'"
cf-configure: ; ssh cf-shell.sf.net \
	ssh $(cf) "'$(cf_profile) cd $(cf_unpacked) && $(cf_configure) 2>&1'"
cf-build: ; ssh cf-shell.sf.net \
	ssh $(cf) "'$(cf_profile) cd $(cf_unpacked) && make 2>&1'"
cf-check: ; ssh cf-shell.sf.net \
	ssh $(cf) "'$(cf_profile) cd $(cf_unpacked) && make check 2>&1'"
cf-clean: ; ssh cf-shell.sf.net \
	ssh $(cf) "'$(cf_profile) cd $(cf_unpacked) && make clean 2>&1'"
cf-distclean: ; ssh cf-shell.sf.net \
	ssh $(cf) "'$(cf_profile) cd $(cf_unpacked) && rm -rf *.d 2>&1'"
cf-prep: dist cf-upload cf-unpack
cf-wipe: ; ssh cf-shell.sf.net \
	ssh $(cf) "'$(cf_profile) rm -rf $(cf_unpacked)/ 2>&1'"
cf-system: ; ssh cf-shell.sf.net \
	ssh $(cf) "'$(cf_profile) $(cf_get_uname) | sed \"s,^,: $(cf) = ,\"'"
cf-configure-all:
	echo $@: `date` "====================" > cf.log
	@ for cf in $($(list)) ; do echo $(MAKE) cf-configure cf=$$cf \
	; sleep 1;                       $(MAKE) cf-configure cf=$$cf \
	| sed -e "s|^|$$cf: |" | tee -a cf.log ; done
cf-build-all:
	echo $@: `date` "====================" >> cf.log
	@ for cf in $($(list)) ; do echo $(MAKE) cf-build cf=$$cf \
	; sleep 1;                       $(MAKE) cf-build cf=$$cf \
	| sed -e "s|^|$$cf: |" | tee -a cf.log ; done
cf-check-all:
	echo $@: `date` "====================" >> cf.log
	@ for cf in $($(list)) ; do echo $(MAKE) cf-check cf=$$cf \
	; sleep 1;                       $(MAKE) cf-check cf=$$cf \
	| sed -e "s|^|$$cf: |" | tee -a cf.log ; done
cf-system-all:
	echo $@: `date` "====================" >> cf.log
	@ for cf in $($(list)) ; do echo $(MAKE) cf-configure cf=$$cf \
	; sleep 1 ;                      $(MAKE) cf-system cf=$$cf \
	| sed -e "s|^|$$cf: |" | tee -a cf.log ; done
