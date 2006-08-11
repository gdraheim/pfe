
# $Id: Makefile.am,v 1.3 2006-08-11 22:56:03 guidod Exp $

pfedir=$(top_srcdir)/pfe
dbkdir=$(top_srcdir)/dbk
# VPATH=$(pfedir) # FIXME: need to add $(srcdir) where appropriate
DOCS=doc
SRCS=src
groups=${infodir}/../groups
docdir=${infodir}/../doc
pkgdocdir=${docdir}/$(PACKAGE)
pkghtdocs=${groups}/$(PACKAGE3)/htdocs
PACKAGE3=p/pf/pfe
PERLXM=perlxm
# v=v

# doc: src/index.html wordsets.tar words.tar htmfiles.tar pfe.1
doc: wordsets.tar words.tar htmfiles.tar pfe.1

../pfe/pfe$(EXEEXT) ../pfe/pfe-config :
	(cd ../pfe && $(MAKE) `basename $@`)

pfe.1 : ../pfe/pfe$(EXEEXT) $(top_srcdir)/pfe.spec
	perl $(srcdir)/help2man.pl ./pfe > $@ || true
	@ test -f $@ || head $(top_srcdir)/pfe.spec > $@

pfe-config.1 : ../pfe/pfe-config
	perl $(srcdir)/help2man.pl "sh ../pfe/pfe-config" > $@ || true
	@ test -f $@ || head $(top_srcdir)/pfe.spec > $@
man : pfe.1 pfe-config.1

install-man : pfe.1 pfe-config.1
	$(mkinstalldirs) $(DESTDIR)$(mandir)/man1
	for i in pfe.1 pfe-config.1 ; do \
	$(INSTALL_DATA) $$i $(DESTDIR)$(mandir)/man1/`echo $$i \
        | sed -e 's/.1$$//' -e '$(transform)' -e 's/$$/.1/'` \
        ; done
install-data-local : install-man

DBKS=   dbk
DBK = 	how-to-write-a-pfe-module.dbk \
	internal-decisions-about-headers.dbk \
	special-implementation-strategies.dbk \
	tuning.dbk literature.dbk license.dbk lgpl-license.dbk \
	dirk-zoller-readme.dbk locals.dbk wordset-modules.dbk \
	make-dist.dbk statesmart-words.dbk known-problems.dbk \
        quoted-parse-idea.dbk porting-sbr-threading.dbk \
        uppercasing.dbk header-fields.dbk
DBK_EXTRA = doc-index.dbk threading.dbk

htm_FILES = pfe.htm index.htm documentation.htm authors.htm license.htm \
            changes.htm lgpl-license.htm
htm_EXTRA = body.htm

# EXTRA_DIST = *.dbk *.htm *.ar *.pl *.pdf pfe.1 *.gif *.css src/Makefile
EXTRA_DIST = $(DBK) $(DBK_EXTRA) $(htm_FILES) $(htm_EXTRA) \
	     dpans.ar 4thtutor.ar help2man.pl Forthprimer.pdf \
	     pfe.1 4ring.gif PFE.css \
             src/Makefile src/PFE.css

WWWTEKFTH=/net/WWW/entwicklung/forth
install-tek: $(DOCS).tar # src/index.html
	( test -d $(WWWTEKFTH)/$(DOCS) || mkdir $(WWWTEKFTH)/$(DOCS) )
	( p=`pwd` ; cd $(WWWTEKFTH)/$(DOCS) && tar xvf $$p/$(DOCS).tar )
	( test -d $(WWWTEKFTH)/$(DOCS)/src || mkdir $(WWWTEKFTH)/$(DOCS)/src )
	cp src/*.html $(WWWTEKFTH)/$(DOCS)/src
	-chmod 775 $(WWWTEKFTH)/$(DOCS)/src
	@ for d in $(WWWTEKFTH)/$(DOCS)/* \
	; do if test -d $$d ; then echo chmod 775 $$d ; chmod 775 $$d ; fi \
	; done ; true
	-chmod 664 $(WWWTEKFTH)/$(DOCS)/*.html
	-chmod 664 $(WWWTEKFTH)/$(DOCS)/*/*.html
	-chmod 664 $(WWWTEKFTH)/$(DOCS)/*.htm
	-chmod 664 $(WWWTEKFTH)/$(DOCS)/*/*.htm
	-chmod 664 $(WWWTEKFTH)/$(DOCS)/*.xml
	-chmod 664 $(WWWTEKFTH)/$(DOCS)/*/*.xml
	-chmod 664 $(WWWTEKFTH)/$(DOCS)/*.gif
	-chmod 664 $(WWWTEKFTH)/$(DOCS)/*/*.gif

# >> pkgdocdir
doc : $(DOCS).tar
install-doc: $(DOCS).tar 
	@ echo ':::' $(DOCS).tar '>>' :$(pkgdocdir)/$(VERSION)
	@ echo mkinstalldirs $(DESTDIR)$(pkgdocdir)/$(VERSION) \
	;            test -d $(DESTDIR)$(pkgdocdir)/$(VERSION) \
	||  $(mkinstalldirs) $(DESTDIR)$(pkgdocdir)/$(VERSION)
	@ echo un-tar '>>' :$(pkgdocdir)/$(VERSION)
	@ p=`pwd` ; cd $(DESTDIR)$(pkgdocdir)/$(VERSION) \
	; tar x$vf $$p/$(DOCS).tar ; cd $$p ; true
	-cp $(top_srcdir)/ChangeLog $(DESTDIR)$(pkgdocdir)/$(VERSION)
	-cp $(top_srcdir)/COPYING.LIB $(DESTDIR)$(pkgdocdir)/$(VERSION)
	@ echo ':::' ln -s $(pkgdocdir)/$(VERSION)/'*' $(pkgdocdir)/$(PACKAGE)/
	@ (cd $(DESTDIR)$(pkgdocdir)/ && ln -s $(VERSION)/* .) || true

# >> pkghtdocs
docs :        $(DOCS).tar $(SRCS).tar ../dbk/index.html
install-docs: $(DOCS).tar $(SRCS).tar ../dbk/index.html
	@ echo ':::' $(DOCS).tar '>>' :$(pkghtdocs)/$(VERSION)
	@ echo mkinstalldirs $(DESTDIR)$(pkghtdocs)/$(VERSION) \
	;            test -d $(DESTDIR)$(pkghtdocs)/$(VERSION) \
	||  $(mkinstalldirs) $(DESTDIR)$(pkghtdocs)/$(VERSION)
	@ echo un-tar '>>' :$(pkghtdocs)/$(VERSION)
	@ (p=`pwd` ; cd $(DESTDIR)$(pkghtdocs)/$(VERSION) \
	; tar x$vf $$p/$(DOCS).tar \
	; tar x$vf $$p/$(SRCS).tar \
	; mkdir manual ; cp $$p/../dbk/*.html manual \
	; cd manual \
	; test -d $(top_srcdir)/dpans && ln -s $(top_srcdir)/dpans .) ; true
	- cp $(top_srcdir)/ChangeLog   $(DESTDIR)$(pkghtdocs)/$(VERSION)
	- cp $(top_srcdir)/COPYING.LIB $(DESTDIR)$(pkghtdocs)/$(VERSION)
	- (cd $(DESTDIR)$(pkghtdocs)/ && ln -s $(VERSION)/* .)

# -------------- sf refdoc -------------

$(DOCS).tar : words.tar wordsets.tar htmfiles.tar
	@ if test -f $(DOCS)/all-$(DOCS).build ; then : \
	; echo rm -r $(DOCS) ; sleep 4 ; rm -r $(DOCS) \
	; fi
	mkdir $(DOCS) ; date > $(DOCS)/all-$(DOCS).build
	@ echo un-tar words.tar '>>' $(DOCS)/words
	@ (cd $(DOCS) && tar xf ../words.tar)
	@ echo un-tar wordsets.tar '>>' $(DOCS)/wordsets
	@ (cd $(DOCS) && tar xf ../wordsets.tar)
	@ echo un-tar htmfiles.tar '>>' $(DOCS)
	@ (cd $(DOCS) && tar xf ../htmfiles.tar)
	@ if test -f $(srcdir)/dpans.ar \
	; then mkdir $(DOCS)/dpans \
	; echo un-ar dpans.ar '>>' $(DOCS)/dpans \
	; cp $(srcdir)/dpans.ar $(DOCS)/dpans \
	; (cd $(DOCS)/dpans && ar x dpans.ar) \
	; (cd $(DOCS)/dpans && ln -s dpans.htm index.html) \
	; rm  $(DOCS)/dpans/dpans.ar \
	; fi ; true
	@ if test -f $(top_srcdir)/dpans/dpansf.html \
	; then mkdir $(DOCS)/dpans \
	; echo cp "../dpans/*" '>>' $(DOCS)/dpans \
	; cp $(top_srcdir)/dpans/* $(DOCS)/dpans \
	; (cd $(DOCS)/dpans && ln -s dpans.htm index.html) \
	; fi ; true
	@ if test -f $(srcdir)/4thtutor.ar \
	; then mkdir $(DOCS)/4thtutor \
	; echo un-ar 4thtutor.ar '>>' $(DOCS)/4thtutor \
	; cp $(srcdir)/4thtutor.ar $(DOCS)/4thtutor \
	; (cd $(DOCS)/4thtutor && ar x 4thtutor.ar) \
	; (cd $(DOCS)/4thtutor && ln -s 4thtutor.htm index.html) \
	; rm  $(DOCS)/4thtutor/4thtutor.ar \
	; fi ; true
	@ if test -f $(srcdir)/Forthprimer.pdf \
	; then : \
	; echo cp Forthprimer.pdf '>>' $(DOCS) \
	; cp $(srcdir)/Forthprimer.pdf $(DOCS)/Forthprimer.pdf \
	; fi ; true
	@ -cp $(srcdir)/*.gif $(DOCS)
	@ echo tar $@ '<<' $(DOCS)
	@ (cd $(DOCS) && tar chf ../$@ .)
	@ echo rm -rf $(DOCS) -- skipped

$(SRCS).tar : Makefile # FIXME make a new variant using xmlg
	mkdir tmp ; (cd tmp && echo "<b>Sorry</b>" >index.html); test -d tmp
	(cd tmp && tar cvf ../$@ *) ; test -d tmp && rm -r tmp ; test -s $@

words.tar : Makefile # FIXME
	mkdir tmp ; (cd tmp && echo "<b>Sorry</b>" >index.html); test -d tmp
	(cd tmp && tar cvf ../$@ *) ; test -d tmp && rm -r tmp ; test -s $@

wordsets.tar : Makefile # FIXME
	mkdir tmp ; (cd tmp && echo "<b>Sorry</b>" >index.html); test -d tmp
	(cd tmp && tar cvf ../$@ *) ; test -d tmp && rm -r tmp ; test -s $@

# -------------- defaults -------------
# should be generated by configure...
sf-html.html : sf-html.html.in
	sed -e 's/@prefix@//g' -e 's/@PACKAGE@//g' $< >$@
sf-news.html : sf-news.html.in
	sed -e 's/@VERSION@/the latest from sourceforge/' $< >$@

clean: clean-doc
clean-docs: clean
clean-doc: 
	rm -f *.html *.xml
	- $(MAKE) -C src clean 
distclean-doc: clean-doc
	if test -f doc/all-doc.build ; then rm -r doc ; fi
	if test -f words/all-words.build ; then rm -r words ; fi
	if test -f wordsets/all-wordsets.build ; then rm -r wordsets ; fi
	if test -f dpans/rfilist.htm ; then rm -r dpans ; fi

# have a look at http://sourceforge.net/projects/xm-tool 
# %.html : %.xml
.xml.html:
	@ $(PERLXM) xm::css::mkhtml4: -- --ext=xml --css=PFE.css $< 
# --------------------------------------------------------- NEW DBK STYLE
SORRY = "Sorry, the doc build system had been missing xmlg from xmlg.sf.net"
../dbk/index.html : $(srcdir)/*.dbk $(dbkdir)/*.dbk $(pfedir)/*.c
	if test -f $(srcdir)/dpans.ar ; then mkdir tmp ; mkdir tmp/dpans \
	; cp $(srcdir)/dpans.ar tmp/dpans \
	; (cd tmp/dpans && ar x dpans.ar && rm dpans.ar)\
	; (cd .. && ln -s doc/tmp/dpans .) ; fi ; true
	test -d ../dbk || mkdir ../dbk
	- test -h $(top_srcdir)/dpans || ln -s `pwd`/tmp/dpans $(top_srcdir)
	(p=`pwd` ; cd $(top_srcdir)/dbk \
	&& $(MAKE) -f pfe-fth.mk index.html pfe-fth.html \
	"DPANS=--dpans=../dpans --nofileref" "OUTPUT= -o $$p/../dbk")
	test -s ../dbk/index.html || echo $(SORRY) > ../dbk/index.html
	- rm -r tmp/dpans || true

# --------------------------------------------------------- NEW HTM STYLE

htmfiles.tar : $(htm_FILES:.htm=.html)
	tar chf $@ $(htm_FILES:.htm=.html)

clean-htmfiles:
	-rm $(DBK:.dbk=.html) $(DBK:.dbk=.xml) index.html

datehead = <hr><table border="0" width="100%"><tr><td> \
    <a href="mailto:guidod@gmx.de?subject=$(PACKAGE):">guidod@gmx.de</a> \
    </td><td align="right">
datetail = </td></tr></table>
borderhead = <table cellpadding="10"><tr><td>
bordertail = </td></tr></table>
DATESPEC=`date +%Y-%m-%d`
VERSIONSPEC=`sed -e '/Version:/!d' -e 's/.*://' $(top_srcdir)/pfe.spec`

htmfiles : $(htm_FILES:.htm=.html)
$(htm_FILES:.htm=.html) : body.htm
# %.html : %.htm
.htm.html:
	@  D=$(DATESPEC) ; V=$(VERSIONSPEC) ; test -z "$$V" && V="$(VERSION)" \
	; title=`sed -e '/<h2>/!d' -e 's:</h2>.*::' \
        -e 's:<[^<>]*>::g' -e 'q' $<` \
	; cat $(srcdir)/body.htm | sed \
	-e "s|<!--VERSION-->|$$V|" \
	-e "s|<!--DATE-->|$$D|" \
	-e "s|</title>| - $$title</title>|" \
	-e "/$@.>/s|\\([^<>]*\\)</a>|<b>\\1</b></a>|" \
	> $@
	@ echo $< '>>' $@; cat $< | sed -e "s/<!--VERSION-->/$(VERSION)/" \
	-e "s:</h2>\\(.*\\):</h2></td><td><i>\\1</i></td></tr></table>:" \
	-e 's:<h2>:<table width="100%"><tr valign="bottom"><td><h2>:' \
	-e 's:<td><i>:<td align="right"><br> <i>:' \
	-e 's:<P>:<p class="justify"><span class="justify">:g' \
	-e 's:</P>:</span></p>:g' \
	-e 's,<date>,<!--DATE: ,' -e 's,</date>, -->,' \
	-e 's,<!--border-->,$(borderhead)<!--BORDER-->,' >>$@
	@ if grep '<!--BORDER-->' $@ >/dev/null ; then : \
	; echo "$(bordertail)<!--/BORDER-->" >>$@ ; fi
	@ D=`grep '<!--DATE:' $@ | sed -e 's,.*<!--DATE: *,,' -e 's, *-->,,'` \
	; if test ".$$D" != "." ; then echo '$(datehead)' >>$@ \
	; echo "$$D" >> $@ ; echo "$(datetail)" >> $@ ;	fi
	@ echo "</td></tr></table></body></html>" >>$@

# PKGHTDOCS=/groups/f/fr/freespace/htdocs/$(PACKAGE)
# PKGDOCDIR=${datadir}${PKGHTDOCS}
PKGHTDOCDIR=${pkghtdocs}
install-htmfiles : $(htm_FILES:.htm=.html) 
	$(mkinstalldirs) $(DESTDIR)$(PKGDOCDIR)
	$(INSTALL_DATA) $(htm_FILES:.htm=.html) $(DESTDIR)$(PKGDOCDIR)
	- (cd $(DESTDIR)$(PKGDOCDIR) && ln -s $(PACKAGE).html index.html)

$(PACKAGE).htm : $(top_srcdir)/$(PACKAGE).spec
	- mv $(PACKAGE).htm $(PACKAGE)-tmp.htm
	- ../pfe/pfe --help    > $(PACKAGE)-tmp.hlp
	- ../pfe/pfe --version > $(PACKAGE)-tmp.ver
	grep -i summary $(PACKAGE).spec | sed -e 's,.*:,,' > $(PACKAGE)-tmp.sum
	echo " <h2> "`cat $(PACKAGE)-tmp.sum` \
	    " </h2> "`cat $(PACKAGE)-tmp.ver` > $@
	echo "<!--border--><date>generated "$(DATESPEC)"</date><pre>" >> $@
	cat $(PACKAGE)-tmp.hlp | sed -e "/^[a-z]/s:.*:<b>&</b>:" >>$@
	echo "</pre>" >> $@
	test -s $@ || mv $(PACKAGE)-tmp.htm $@
	- rm $(PACKAGE)-tmp.*

changes.htm : $(top_srcdir)/ChangeLog
	echo "<pre>" > $@ ; cat $< | sed -e "s,\\&,\\&amp\\;,g" \
	-e "s,<,\\&lt\\;,g" -e "s,>,\\&gt\\;,g" \
	-e "/^[0-9]/s,\\(.*\\),<b>&</b>," >> $@ ; echo "</pre>" >>$@


