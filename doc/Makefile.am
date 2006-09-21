
# $Id: Makefile.am,v 1.10 2006-09-21 01:34:16 guidod Exp $

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
XMLTO= xmlto $(XMLTO_FLAGS)
# v=v

# doc: src/index.html wordsets.tar words.tar htmfiles.tar pfe.1
# doc: wordsets.tar words.tar htmfiles.tar pfe.1
docu: man3

../pfe/pfe$(EXEEXT) ../pfe/pfe-config :
	(cd ../pfe && $(MAKE) `basename $@`)

pfe.1 : ../pfe/pfe$(EXEEXT) $(top_srcdir)/pfe.spec
	perl $(srcdir)/help2man.pl ../pfe/pfe$(EXEEXT) > $@ || true
	@ test -f $@ || head $(top_srcdir)/pfe.spec > $@

pfe-config.1 : ../pfe/pfe-config
	perl $(srcdir)/help2man.pl "sh ../pfe/pfe-config" > $@ || true
	@ test -f $@ || head $(top_srcdir)/pfe.spec > $@
mans : pfe.1 pfe-config.1 man3

install-mans : install-man1 install-man3
install-man1 : pfe.1 pfe-config.1 
	$(mkinstalldirs) $(DESTDIR)$(mandir)/man1
	for i in pfe.1 pfe-config.1 ; do \
	$(INSTALL_DATA) $$i $(DESTDIR)$(mandir)/man1/`echo $$i \
        | sed -e 's/.1$$//' -e '$(transform)' -e 's/$$/.1/'` \
        ; done
install-man3 : pfe-man3.ar
	$(mkinstalldirs) $(DESTDIR)$(mandir)/man3
	(p=`pwd` ; cd $(DESTDIR)$(mandir)/man3 && ar x $$p/pfe-man3.ar)
install-data-local : install-mans

DBKS=   dbk
DBK = 	how-to-write-a-pfe-module.dbk \
	internal-decisions-about-headers.dbk \
	special-implementation-strategies.dbk \
	tuning.dbk literature.dbk license.dbk lgpl-license.dbk \
	dirk-zoller-readme.dbk locals.dbk wordset-modules.dbk \
	make-dist.dbk statesmart-words.dbk known-problems.dbk \
        quoted-parse-idea.dbk porting-sbr-threading.dbk \
        uppercasing.dbk header-fields.dbk
DBK_EXTRA = doc-index.dbk threading.dbk pfe-manual.dbk

htm_FILES = pfe.htm index.htm documentation.htm authors.htm license.htm \
            changes.htm lgpl-license.htm
htm_EXTRA = body.htm

pfedoc_py_DIST = makedocs.py \
                 pfedoc/__init__.py  pfedoc/match.py  pfedoc/options.py \
                 pfedoc/functionheader.py        pfedoc/textfileheader.py \
                 pfedoc/functionprototype.py     pfedoc/textfile.py \
                 pfedoc/functionlisthtmlpage.py  pfedoc/commentmarkup.py \
                 pfedoc/functionlistreference.py pfedoc/dbk2htm.py \
                 pfedoc/forthheader.py           pfedoc/forthnotation.py \
                 pfedoc/forthwordset.py          pfedoc/htmldocument.py  \
                 pfedoc/forthwords.py            pfedoc/htmldirectory.py \
                 pfedoc/forthwordsethtmlpage.py  pfedoc/docbookdocument.py \
                 pfedoc/forthwordsetreference.py

# EXTRA_DIST = *.dbk *.htm *.ar *.pl *.pdf pfe.1 *.gif *.css src/Makefile
EXTRA_DIST = $(DBK) $(DBK_EXTRA) $(htm_FILES) $(htm_EXTRA) \
	     dpans.ar 4thtutor.ar help2man.pl Forthprimer.pdf \
	     pfe.1 4ring.gif PFE.css \
             src/Makefile src/PFE.css \
             $(pfedoc_py_DIST)

# >> pkgdocdir
docs doc : $(DOCS).tar
install-docs install-doc: $(DOCS).tar 
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
SFHTDOCS=/home/groups/p/pf/pfe/htdocs/
install-sf:
	$(MAKE) install-docs DESTDIR=/tmp/pfe-doc-sourceforge-$$USER/
	sleep 4
	scp -r /tmp/pfe-doc-sourceforge-$$USER/$(pkgdocdir)/$(VERSION) \
        shell.sf.net:$(SFHTDOCS)
	ssh shell.sf.net "cd $(SFHTDOCS)/ && ln -sf $(VERSION)/* ."
	rm  -r /tmp/pfe-doc-sourceforge-$$USER/

# -------------- sf refdoc -------------

$(DOCS).tar : htmfiles.tar pfe-wordsets.tar pfe-manual.tar \
              pfe-wordsets.html pfe-words.html
	@ if test -f $(DOCS)/all-$(DOCS).build ; then : \
	; echo rm -r $(DOCS) ; sleep 4 ; rm -r $(DOCS) \
	; fi
	mkdir $(DOCS) ; date > $(DOCS)/all-$(DOCS).build
	@ echo un-tar htmfiles.tar '>>' $(DOCS) \
	; cd $(DOCS) && tar xf ../htmfiles.tar
	@ echo un-tar pfe-wordsets.tar '>>' $(DOCS) \
	; cd $(DOCS) && tar xf ../pfe-wordsets.tar
	@ echo un-tar pfe-manual.tar '>>' $(DOCS) \
	; cd $(DOCS) && tar xf ../pfe-manual.tar
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
	-cp $(srcdir)/*.gif $(DOCS)
	- cp pfe-wordsets.html pfe-words.html $(DOCS)
	@ echo tar $@ '<<' $(DOCS)
	@ (cd $(DOCS) && tar chf ../$@ .)
	@ echo rm -rf $(DOCS) -- skipped

clean: clean-doc
clean-docs clean-doc: 
	rm -f *.html *.xml

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

pfe_words_OPT=
PYRUN= $(PYTHONFLAGS) python $(PYTHONOPTIONS)
docus : pfe-wordsets.html pfe-words.html pfe-words.docbook
pfe-wordsets.html pfe-words.html pfe-words.docbook : \
        $(srcdir)/*.dbk $(dbkdir)/*.dbk $(pfedir)/*.c \
        $(srcdir)/*.py $(srcdir)/pfedoc/*.py $(srcdir)/*.am
	$(PYRUN) $(srcdir)/makedocs.py $(pfedir)/*.c \
        "--package=$(PACKAGE)" "--version=$(VERSION)" \
        "--body=$(srcdir)/body.htm" $(pfe_words_OPT) 
man3: pfe-man3.ar
pfe-man3.ar : pfe-words.docbook
	- test ! -d man3  || mkdir man3; rm man3/*
	$(XMLTO) -o man3 man pfe-words.docbook
	ar -q "$@" man3/*
pfe-wordsets.tar : pfe-wordsets.html
	tar cf "$@" pfe-wordsets/*
pfe-manual.docbook : pfe-wordsets.docbook $(DBK) $(DBK_EXTRA)
	sed -e "s|../doc/|$(srcdir)/|" $(srcdir)/doc-index.dbk   \
	> doc-index.docbook
	sed -e "s|../doc/|$(srcdir)/|" $(srcdir)/pfe-manual.dbk  \
	-e 's|"[^"]*/doc-index.dbk"|"doc-index.docbook"|' \
	> pfe-manual.docbook
pfe-manual/index.html : pfe-manual.docbook
	- test ! -d pfe-manual  || mkdir pfe-manual; rm pfe-manual/*
	$(XMLTO) --skip-validation -o pfe-manual html pfe-manual.docbook
pfe-manual.tar: pfe-manual/index.html
	tar cf "$@" pfe-manual/*
pfe-manual.pdf : pfe-manual.docbook
	$(XMLTO) --skip-validation pdf pfe-manual.docbook
pdfs: pfe-manual.pdf

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
        -e "/body>.*html>/d" \
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
	@ cat $(srcdir)/body.htm | sed \
        -e "/body>.*html>/!d" \
	>>$@

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


