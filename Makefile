#! make -f

PFECONFIGURE=$(CONFIGUREFLAGS)
DISTPACKAGE=pfe
SUBDIRS = pfe doc docbook test
BUILDDIRS = Debug Release
DISTFILES = Makefile COPYING.LIB Changelog INSTALL README TODO \
	pfe.1   config.guess config.sub configure \
	install-sh ltconfig ltmain.sh missing mkbins.cmd mkinstalldirs

CC=gcc
NULL=/dev/null
uname_psr= $(CC) -dumpmachine 2>$(NULL)

default: all

# config.date is not shipped by the tarball but made by toplevel configure
config.date : pfe/configure pfe/Makefile.in
	sh configure $(PFECONFIGURE) # config.guess

pfe/Makefile.in pfe/configure : pfe/Makefile.am pfe/configure.in
	cd pfe && autoconf && automake && autoheader
#	cd pfe && aclocal && autoconf && automake && autoheader

# standard makefile targets to propagate to build directories => single
 pfe libpfe.la module.la edit.la toolbelt.la struct.la structs.la clean-tests \
 info install-exec install-data install-strip mostlyclean maintainer-clean \
man install-man doc install-doc docs install-docs install-scripts \
all install clean config reconfigure tests uninstall distclean check :
	@ D="$(HOST)" ; test -z `$(uname_psr)` || D=`$(uname_psr)` || true \
	; test ! -z "$$D" || D=`sh config.guess` || true ;            r="" \
	; for d in Release/$$D Debug/$$D \
	; do test -f "$$d/Makefile" && $(MAKE) -C $$d $@ \
	&& r="$$r $$d" \
	; done ; echo $@'`d' : $$r ; test ! -z "$$r"

# standard makefile targets to propagate to build directories => -all
pfe-all clean-tests-all tests-all reconfigure-all \
install-all-exec install-all-data install-all-strip \
mostlyclean-all maintainer-clean-all distclean-all \
man-all install-all-man doc-all install-doc-all docs-all install-docs-all \
all-am install-all clean-all config-all uninstall-all check-all :
	@ D="$(HOST)" ; test -z `$(uname_psr)` || D=`$(uname_psr)` || true \
	; test ! -z "$$D" || D=`sh config.guess` || true ;            r="" \
	; for d in Release/$$D-* Debug/$$D-* \
	; do test -f "$$d/Makefile" \
	&& $(MAKE) -C $$d `echo $@ | sed -e 's/-all//'` \
	&& r="$$r $$d" \
	; done ; echo $@'`d' : $$r ; test ! -z "$$r"

# some makefile targets to propagate to doc/Makefile
doc/words.tar doc/wordsets.tar doc/sfbasics.tar doc/doc.tar doc/pfe.1 \
clean-docs clean-doc distclean-doc \
words.tar wordsets.tar sfbasics.tar doc.tar pfe.1 :
	$(MAKE) $(MAKEFLAGS) -C doc `basename $@`

# some makefile targets to propagate to test/Makefile
test : 
	-rm test/*.ok
	$(MAKE) $(MAKEFLAGS) -C test $@

# and some additional dependencies
clean mostlyclean : clean-local
distclean maintainer-clean : distclean-local
all:   config.date

all2:   # if you have a 2 processor machine
	$(MAKE) $(MAKEFLAGS) -j 2

all4:   # if you have a 4 processor machine
	$(MAKE) $(MAKEFLAGS) -j 4

# extra-clean will even remove the latest build-directories
extra-clean: clean-local
	@ D="$(HOST)" ; if ($(uname_psr)) ; then D=`$(uname_psr)` ; fi \
	; test ! -z "$D" || D=`sh config.guess` || true ;         r="" \
	; for d in Release/$$D Debug/$$D Release/$$D-* Debug/$$D-* \
	; do test -f "$$d/Makefile" && rm -rf $$d ; done \
	; rm -f config.date ; true

clean-local:
	rm -f *~ */*~ *.bak */*.bak core */core config.date

distclean-local: clean-local distclean-doc
	rm -f */*/*.lo */*/*.o ; rm -rf */*/.libs rm -rf */*/.deps
	rm -f */*/config.log  */config.log */*/libtool* */libtool
	rm -rf Release/cross-mingw
	(cd pfe && rm -f config.h pfe-config.h pfe-target.h Makefile)

#  ----------------------------------------------------------------- #
auto:
	cd pfe && aclocal && automake && autoconf && autoheader



DISTFINDFILES = $(DISTFILES) \*.h \*.c \*.in \*.am \*.m4 \
	\*.pl \*.pm \*.xm \*.dbk \*.css \*.sh \*.readme \*.spec \
	\*.chk \*.ar \*.txt \*.TXT *.gif \*.pdf \
	\*.f \*.fr \*.fs \*.4th \*.fth \*.seq \*.blk

zipfiles:
	(for i in $(DISTFINDFILES) ; do find . -name "$$i" ; done) \
	| xargs echo $(DISTFILES) 

TARBALL=$(DISTPACKAGE)-current
$(TARBALL).zip: distclean-local
$(TARBALL).zip: $(DISTFILES) pfe.* pfe/*.* test/*.* doc/*.* doc/*/*.* 
	@ test ! -f $@ || rm $@
	(for i in $(DISTFINDFILES) ; do find . -name "$$i" ; done) \
	| grep -v /old/ | sort | uniq | zip -9 $@ -@

$(TARBALL).tgz: $(TARBALL).zip
	@ ver=`grep -i "^version:" pfe.spec | head -1 | tr -dc "0-9."` \
	; test ! -z "$$ver" || ver=`date +%Y.%m.%d` \
	; pkg=$(DISTPACKAGE)-$$ver \
	; rm -rf _p_a_c_k_ ; mkdir _p_a_c_k_ && mkdir _p_a_c_k_/$$pkg \
	; echo "unzip $$pkg" \
	; cd _p_a_c_k_/$$pkg && unzip -q ../../$(TARBALL).zip \
	; echo "tar $$pkg" \
	; cd .. && tar chlf ../_p_a_c_k_.tar $$pkg \
	; cd .. && rm -rf _p_a_c_k_ \
	; echo "gzip $$pkg" \
	; cat _p_a_c_k_.tar | gzip -9 >$@ \
	; rm -f _p_a_c_k_.tar

zip: $(TARBALL).zip
	@ pub=. ; for i in ../../pub ../pub pub \
	; do if test -d $$i ; then pub=$$i ; fi ; done \
	; ver=`date +%m%d` ; pkg=$(DISTPACKAGE)-$$ver \
	; echo mv $(TARBALL).zip $$pub/$$pkg.zip \
	; mv $(TARBALL).zip $$pub/$$pkg.zip

dist: $(TARBALL).tgz
	@ pub=. ; for i in ../../pub ../pub pub \
	; do if test -d $$i ; then pub=$$i ; fi ; done \
	; ver=`grep -i "^version:" pfe.spec | head -1 | tr -dc "0-9."` \
	; test ! -z "$$ver" || ver=`date +%Y.%m%d` \
	; pkg=$(DISTPACKAGE)-$$ver \
	; echo cp "   >" $$pub/$$pkg.tar.gz && cp $? $$pub/$$pkg.tar.gz \
	; echo bzip2 ">" $$pub/$$pkg.tar.bz2 \
	; gzip -dc $? | bzip2 -9 >$$pub/$$pkg.tar.bz2

dist-doc docdist: doc/doc.tar
	@ pub=. ; for i in ../../pub ../pub pub \
	; do if test -d $$i ; then pub=$$i ; fi ; done \
	; ver=`grep -i "^version:" pfe.spec | head -1 | tr -dc "0-9."` \
	; test ! -z "$$ver" || ver=`date +%Y.%m%d` \
	; pkg=$(DISTPACKAGE)-doc-$$ver \
	; tar=doc/doc.tar \
	; echo cp "  <<" $$tar && cp $$tar $$pub/$$pkg.tar \
	; echo bzip2 ">" $$pub/$$pkg.tar.bz2 \
	; bzip2 --keep -9 $$pub/$$pkg.tar \
	; echo gzip " >" $$pub/$$pkg.tar.gz \
	; gzip -9 $$pub/$$pkg.tar

cross-mingw :
	cross-configure.sh --disable-shared
	cross-make.sh
	test -d Release/cross-mingw || mkdir Release/cross-mingw
	p=`pwd` && cross-make.sh install DESTDIR=$$p/Release/cross-mingw
	cd Release/cross-mingw/programs/pfe && mv i*-pfe pfe.exe
	@ pac=. ; for i in ../../packages ../packages packages \
	; do if test -d $$i ; then pac=$$i ; fi ; done \
	; ver=`grep -i "^version:" pfe.spec | head -1 | tr -dc "0-9"` \
	; test ! -z "$$ver" || ver=`date +%Y.%m%d` \
	; pkg=$(DISTPACKAGE)-mingw32-$$ver \
	; cd Release/cross-mingw && zip -9r ../../$$pkg.zip . \
	; cd .. ; rm -r cross-mingw \
	; echo created $$pkg.zip

cross-mingw-dll :
	cross-configure.sh --with-regs
	cross-make.sh
	test -d Release/cross-mingw || mkdir Release/cross-mingw
	@ for i in Release/*/pfe ; do if grep pfe.exe $$i \
	; then (cd `dirname $$i` ; echo ln -s pfe pfe.exe OBSOLETE) \
	; fi done
	p=`pwd` && cross-make.sh install DESTDIR=$$p/Release/cross-mingw
	cd Release/cross-mingw/programs/pfe \
	&& mv i*-pfe.exe pfe.exe 
	@ pac=. ; for i in ../../packages ../packages packages \
	; do if test -d $$i ; then pac=$$i ; fi ; done \
	; ver=`grep -i "^version:" pfe.spec | head -1 | tr -dc "0-9"` \
	; test ! -z "$$ver" || ver=`date +%Y.%m%d` \
	; pkg=$(DISTPACKAGE)-mingw32-$$ver \
	; cd Release/cross-mingw && zip -9r ../../$$pkg.zip . \
	; cd .. ; rm -r cross-mingw \
	; echo created $$pkg.zip

installpackage install-tgz : config.date
	@ pac=. ; for i in ../../packages ../packages packages \
	; do if test -d $$i ; then pac=$$i ; fi ; done \
	; ver=`grep -i "^version:" pfe.spec | head -1 | tr -dc "0-9"` \
	; test ! -z "$$ver" || ver=`date +%Y.%m%d` \
	; pk=$(DISTPACKAGE)-`sh config.guess` \
	; pkg=$(DISTPACKAGE)-`sh config.guess`-$$ver \
	; test -d Release/$$pk || mkdir Release/$$pk \
	; date >Release/$$pk.installpackage \
	; D="$(HOST)" ; test -z `$(uname_psr)` || D=`$(uname_psr)` || true \
	; test ! -z "$$D" || D=`sh config.guess` || true ; topdir=`pwd` \
	; $(MAKE) -C Release/$$D DESTDIR=$$topdir/Release/$$pk install \
	; echo tar $$pkg.tar '<<' Release/$$pk \
	; (cd Release && tar chf ../$$pkg.tar $$pk) \
	; echo gzip $$pkg.tar '>>' $$pkg.tgz \
	; gzip -9 $$pkg.tar ; mv $$pkg.tar.gz $$pkg.tgz \
	; echo created $$pkg.tgz \
	; if test -f $$pkg.tgz \
	; then rm -r Release/$$pk ; rm Release/$$pk.installpackage \
	; fi \
	; true
clean-installpackage clean-install-tgz:
	@ for i in Release/*.installpackage \
	; do if test -f $$ i ; then echo rm $$i \
	; (cd Release && rm -r `basename $$i .installpackage`) \
	; rm $$i \
	; fi done

rpm: $(TARBALL).tgz
	@ pac=. ; for i in ../../packages ../packages packages \
	; do if test -d $$i ; then pac=$$i ; fi ; done \
	; ver=`grep -i "^version:" pfe.spec | head -1 | tr -dc "0-9."` \
	; test ! -z "$$ver" || ver=`date +%Y.%m%d` \
	; pkg=$(DISTPACKAGE)-$$ver \
	; echo cp $? $$pac/SOURCES/$$pkg.tar.gz \
	; cp $? $$pac/SOURCES/$$pkg.tar.gz
	rpm -ba pfe.spec







