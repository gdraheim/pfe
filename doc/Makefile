
srcdir=../pfe
VPATH=$(srcdir)
DOCS=doc
SRCS=src
prefix=/usr/local
exec_prefix=${prefix}
PACKAGE=pfe

doc: src/index.html wordsets.tar words.tar sfbasics.tar pfe.1

pfe.1 : ../pfe.spec
	perl help2man.pl pfe > $@

src/index.html: $(srcdir)/*.c
	$(MAKE) -C src these

src/index.xml src/all-fcode.xml src/all-wordsets.xml : $(srcdir)/*.c
	$(MAKE) -C src `basename $@`

DBK = 	how-to-write-a-pfe-module.dbk \
	internal-decisions-about-headers.dbk \
	special-implementation-strategies.dbk \
	tuning.dbk literature.dbk license.dbk lgpl-license.dbk \
	dirk-zoller-readme.dbk \
	make-dist.dbk statesmart-words.dbk

WWWTEKFTH=/net/WWW/entwicklung/forth
install-tek: $(DOCS).tar src/index.html
	test -d $(WWWTEKFTH)/$(DOCS) || mkdir $(WWWTEKFTH)/$(DOCS)
	( p=`pwd` ; cd $(WWWTEKFTH)/$(DOCS) && tar xvf $$p/$(DOCS).tar )
	test -d $(WWWTEKFTH)/$(DOCS)/src || mkdir $(WWWTEKFTH)/$(DOCS)/src )
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

SFMEAT= # guidod@
WWWPFE=/home/groups/pfe/htdocs
SFHOST=shell.sourceforge.net
install-sf: $(DOCS).tar $(SRCS).tar
	@ echo scp $(DOCS).tar $(SRCS).tar '>>' sourceforge
	@ scp -C $(DOCS).tar $(SRCS).tar $(SFMEAT)$(SFHOST):$(WWWPFE)
	@ echo ssh sourcforge '>>' un-tar
	ssh $(SFMEAT)$(SFHOST) \
	" cd $(WWWPFE) ; tar xvf $(DOCS).tar ; tar xvf $(SRCS).tar \
        ; cd ; etc/index-projdata.sh "

doc : $(DOCS).tar
install-doc: $(DOCS).tar 
	@ echo cp $(DOCS).tar '>>' :$(exec_prefix)/doc/$(PACKAGE)
	@ test -d $(DESTDIR)$(exec_prefix)/doc \
	||  mkdir $(DESTDIR)$(exec_prefix)/doc
	@ test -d $(DESTDIR)$(exec_prefix)/doc/$(PACKAGE) \
	||  mkdir $(DESTDIR)$(exec_prefix)/doc/$(PACKAGE)
	@ echo un-tar '>>' :$(exec_prefix)/doc/$(PACKAGE)
	@ p=`pwd` ; cd $(DESTDIR)$(exec_prefix)/doc/$(PACKAGE) \
	; tar xvf $$p/$(DOCS).tar ; cd $$p ; true
	cp ../Changelog ../COPYING.LIB $(DESTDIR)$(exec_prefix)/doc/$(PACKAGE)

docs : $(DOCS).tar $(SRCS).tar
install-docs: $(DOCS).tar $(SRCS).tar 
	@ echo cp $(DOCS).tar $(SRCS).tar '>>' :$(exec_prefix)/doc/$(PACKAGE)
	@ test -d $(DESTDIR)$(exec_prefix)/doc \
	||  mkdir $(DESTDIR)$(exec_prefix)/doc
	@ test -d $(DESTDIR)$(exec_prefix)/doc/$(PACKAGE) \
	||  mkdir $(DESTDIR)$(exec_prefix)/doc/$(PACKAGE)
	@ echo un-tar '>>' :$(exec_prefix)/doc/$(PACKAGE)
	@ p=`pwd` ; cd $(DESTDIR)$(exec_prefix)/doc/$(PACKAGE) \
	; tar xvf $$p/$(DOCS).tar ; tar xvf $$p/$(SRCS).tar ; cd $$p ; true

# -------------- basics tar ------------

sfbasics.tar : $(DBK:.dbk=.xml) doc-index.xml 
	xm-perl xm::navheaderhrefsfromindex doc-index.xml 
	$(MAKE) $(DBK:.dbk=.html) doc-index.html \
		sf-index.html sf-news.html sf-html.html
	sed -e "/-- sf-html -->/r sf-html.html" \
	    -e "/-- sf-news -->/r sf-news.html" \
	    <sf-index.html >index.html
	tar chf $@ $(DBK:.dbk=.html) doc-index.html \
		sf-index.html sf-news.html sf-html.html index.html

clean-basics:
	-rm $(DBK:.dbk=.html) $(DBK:.dbk=.xml) doc-index.html \
		sf-index.html sf-news.html sf-html.html index.html

# -------------- sf refdoc -------------

$(DOCS).tar : words.tar wordsets.tar sfbasics.tar \
	index.html sf-index.html
	@ if test -f $(DOCS)/all-$(DOCS).build ; then : \
	; echo rm -r $(DOCS) ; sleep 4 ; rm -r $(DOCS) \
	; fi
	mkdir $(DOCS) ; date > $(DOCS)/all-$(DOCS).build
	@ echo un-tar words.tar '>>' $(DOCS)/words
	@ (cd $(DOCS) && tar xf ../words.tar)
	@ echo un-tar wordsets.tar '>>' $(DOCS)/wordsets
	@ (cd $(DOCS) && tar xf ../wordsets.tar)
	@ echo un-tar sfbasics.tar '>>' $(DOCS)
	@ (cd $(DOCS) && tar xf ../sfbasics.tar)
	@ if test -f dpans.ar \
	; then mkdir $(DOCS)/dpans \
	; echo un-ar dpans.ar '>>' $(DOCS)/dpans \
	; (cd $(DOCS)/dpans && ar x ../../dpans.ar \
	&& ln -s dpans.htm index.html) \
	; fi ; true
	@ if test -f 4thtutor.ar \
	; then mkdir $(DOCS)/4thtutor \
	; echo un-ar 4thtutor.ar '>>' $(DOCS)/4thtutor \
	; (cd $(DOCS)/4thtutor && ar x ../../4thtutor.ar \
	&& ln -s 4thtutor.htm index.html) \
	; fi ; true
	@ if test -f Forthprimer.pdf \
	; then : \
	; echo cp Forthprimer.pdf '>>' $(DOCS) \
	; cp Forthprimer.pdf $(DOCS)/Forthprimer.pdf \
	; fi ; true
	@ -cp *.gif $(DOCS)
	@ echo tar $@ '<<' $(DOCS)
	@ (cd $(DOCS) && tar chf ../$@ .)
	@ echo rm -rf $(DOCS) -- skipped

$(SRCS).tar : src/all-wordsets.xml src/index.html
	@ for i in $(SRCS)/*.xml \
	; do case $$i in all-*) \
	;; *) $(MAKE) `echo $$i | sed -e 's/.xml$$/.html/'` \
	;; esac \
	; done
	@ tar chf $@ $(SRCS)/*.html
	@ echo rm '$(SRCS)/*.html -- skipped'

# -------------- wordsets -------------

all-wordsets.xml : src/all-wordsets.xml
	@ sed -e 's:</*ALLWORDSETS>::' $< \
	| xm-perl xm::pfe::flattenwordrefs - >$@

cat-wordsets.xml : all-wordsets.xml src/all-fcode.xml
	@ cat all-wordsets.xml src/all-fcode.xml \
	| xm-perl xm::pfe::expandxrefwordrefsfcode - >$@

index-wordsets.xml : cat-wordsets.xml
	@ list=` xm-perl xm::pfe::wordset2index cat-wordsets.xml ` \
	; xm-perl xm::navheaderhrefsfromindex $@ \
	; for i in $$list \
	; do echo $$i:html ; $(MAKE) `basename $$i .xml`.html ; done \
	; test -s $@

index-words.xml : cat-wordsets.xml
	@ list=` xm-perl xm::pfe::wordset2words cat-wordsets.xml ` \
	; xm-perl xm::navheaderhrefsfromindex $@ \
	; for i in $$list \
	; do echo $$i:html ; $(MAKE) `basename $$i .xml`.html ; done \
	; test -s $@

words.tar : index-words.html
	@ if test -f words/all-words.build ; then : \
	; echo rm -r words ; sleep 4 ; rm -r words \
	; fi 
	mkdir words ; date > words/all-words.build
	@ for i in w-*.html w-*.xml \
	; do if test -f "$$i" \
	; then sed -e 's/index-words.html/index.html/' $$i >words/$$i \
	; echo rm $$i ; rm $$i \
	; fi done
	@ echo keep index-words.xml index-words.html '(cp words/index.html)'
	@ cp index-words.xml  words/index.xml  # do not break dependency
	@ cp index-words.html words/index.html # for the tar-file !
	@ tar chf $@ words
	rm -r words

wordsets.tar : index-wordsets.html
	@ if test -f wordsets/all-wordsets.build ; then : \
	; echo rm -r wordsets ; sleep 4 ; rm -r wordsets \
	; fi 
	mkdir wordsets ; date > wordsets/all-wordsets.build
	@ for i in wordset-*.html wordset-*.xml \
	; do if test -f "$$i" \
	; then sed -e 's/index-wordsets.html/index.html/' $$i >wordsets/$$i \
	; fi done
	@ cp index-wordsets.xml  wordsets/index.xml  # do not break dependency
	@ cp index-wordsets.html wordsets/index.html # for the tar-file !
	@ tar chf $@ wordsets
	rm -r wordsets

# -------------- defaults -------------
# should be generated by configure...
sf-html.html : sf-html.html.in
	sed -e 's/@prefix@//g' -e 's/@PACKAGE@//g' $< >$@
sf-news.html : sf-news.html.in
	sed -e 's/@VERSION@/the latest from sourceforge/' $< >$@

clean-docs: clean
clean-doc: clean
	rm -f *.html *.xml
	$(MAKE) -C src clean 
clean: clean-doc
distclean-doc: clean-doc
	if test -f doc/all-doc.build ; then rm -r doc ; fi
	if test -f words/all-words.build ; then rm -r words ; fi
	if test -f wordsets/all-wordsets.build ; then rm -r wordsets ; fi
	if test -f dpans/rfilist.htm ; then rm -r dpans ; fi

sfdoc: sfdoc.tar.gz
sfdoc.tar.gz : doc
	tar czvf $@ *.in */*.in *.html */*.html

# -------------- rules ----------------

# have a look at http://sourceforge.net/projects/xm-tool 
%.xml : %.c
	@ xm-perl xm::cread              $< \
	| xm-perl xm::cdocs              - \
	| xm-perl xm::cblocks            - \
	| xm-perl xm::cdefs              - \
	| xm-perl xm::pfe::grabfcode     - \
	| xm-perl xm::pfe::grabwordset   - \
	| xm-perl xm::pfe::grab4compiles - \
	| xm-perl xm::grabitemxdoc       - \
	| xm-perl xm::pfe::xdefdoc       - \
	| xm-perl xm::grabitemincl       - \
	| xm-perl xm::grabitemxtra       - > $@ 
	@ echo $@

%.xml : %.dbk
	@ xm-perl xm::listing $< \
	| xm-perl xm::colontags - \
	| xm-perl xm::pp_maybe - \
	| xm-perl xm::umlaut - \
	> $@
	@ echo $@

%.html : %.xml
	@ xm-perl xm::css::mkhtml4 -- --ext=xml --css=PFE.css $< 

