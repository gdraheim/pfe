BOOK = pfe-fth
WORDS = pfe-wordsets
PARTS = $(WORDS).docbook $(WORDS).reference
MAKES= $(MAKE) -f $(BOOK).mk

CCM_SUBPROJTOOL_XSL=../TOOL_XSL
TOOL_XSL=$(CCM_SUBPROJTOOL_XSL)

chm : chm-user/$(BOOK).chm

chm-user/$(BOOK).chm : *.dbk *.part ../doc/*.dbk $(TOOL_XSL)/*
	$(MAKE) -f $(TOOL_XSL)/makefile.chm SEC=user DOC=$(BOOK)

html : html-internal/$(BOOK).tar.gz

html-internal/$(BOOK).tar.gz : *.dbk *.part ../doc/*.dbk $(TOOL_XSL)/*
	$(MAKE) -f $(TOOL_XSL)/makefile.html SEC=internal DOC=$(BOOK)

htmls : $(PARTS)
	test -f dpans-toc.part || echo "" >dpans-toc.part
	$(MAKES) html
	test -s dpans-toc.part || rm dpans-toc.part

htm : pfe-fth.html
pfe-fth.html : *.dbk $(PARTS) ../doc/*.dbk
	test -f dpans-toc.part || echo "" >dpans-toc.part
	xmlto html-nochunks pfe-fth.dbk $(OUTPUT)
	test -s dpans-toc.part || rm dpans-toc.part

htms : index.html
index.html : *.dbk $(PARTS) ../doc/*.dbk
	test -f dpans-toc.part || echo "" >dpans-toc.part
	xmlto html pfe-fth.dbk $(OUTPUT)
	test -s dpans-toc.part || rm dpans-toc.part

# check for unresolved internal links in pfe-words master
unresolved : 
	grep "link  linkend" $(PARTS)

# check for errors in syntax to put documentation into source code
check-mforth :
	grep '^[/][*][ ]' ../mforth/*.c \
	| grep '(' \
	| grep -v -e '----' \
	| grep -v -e '====' \
	| grep -v -e '[.][.][.][.][.]'
# known problems SENDME-CR-QUERY defined twice, the badformatted one
# tells about "internal" implementation behaviour.

check-pfe :
	grep '^[/][*][ ]' ../EMUL_PFE/pfe/*.c \
	| grep '(' \
	| grep -v -e '----' \
	| grep -v -e '====' \
	| grep -v -e '[.][.][.][.][.]'

watch-chm :
	/net/guidod/bin/watch " ls -t -l chm-user | head "
watch-html :
	/net/guidod/bin/watch " ls -t -l html-internal | head "

#_______________ making of $(PARTS) :        (note: you need lib pcre!)
EMUL_PFE=..

$(WORDS).refs.xml $(WORDS).part.xml : $(EMUL_PFE)/pfe/*.c  \
	         wordsets2words.pl # xml-c-pfe-doc
	- test -f ./xml-c-pfe-doc && cat ./xml-c-pfe-doc > ./xml-c-pfe-doc.e
	- test -s ./xml-c-pfe-doc.e || \
	echo '#! /usr/bin/env xml-c-pfe-doc' >./xml-c-pfe-doc.e
	chmod +x ./xml-c-pfe-doc.e
	./xml-c-pfe-doc.e $(EMUL_PFE)/pfe/*.c > pfe-words.xml
	perl ./wordsets2words.pl --indexterm $(DPANS) pfe-words.xml \
             --wordsets > $(BOOK).part.xml
	perl ./wordsets2words.pl --indexterm $(DPANS) pfe-words.xml \
            --reference > $(BOOK).refs.xml
$(WORDS).docbook   : $(WORDS).part.xml
	test -s $< && cp $< $@
$(WORDS).reference : $(WORDS).refs.xml
	test -s $< && cp $< $@

# --prefix=$HOME/$OSTYPE is my usual installplace
MY_EMUL_PFE=/net/guidod/a/pfe
MY_USRLOCAL=/net/guidod/solaris2.6
MY_XMLG=/net/guidod/4/xmlg
XMLG=xmlg
USRLOCAL=/usr/local
xmlgconfigure :
	( cd "$(XMLG)" && sh configure --disable-shared \
        --prefix=$(USRLOCAL) --with-pcre=$(USRLOCAL) \
        --with-pfe=$(EMUL_PFE) )
xmlgmake :
	cd "$(XMLG)" && make
xmlgclean :
	cd "$(XMLG)" && make clean
xmlgcopy :
	test -f "$(XMLG)/bin/xml-c-pfe-doc" && \
	     cp "$(XMLG)/bin/xml-c-pfe-doc" ./xml-c-pfe-doc
	ls -lL xml-c-pfe-doc
xmlgcopy2 :
	test -f "$(XMLG)/doc/pfe/wordsets2words.pl" && \
	     cp "$(XMLG)/doc/pfe/wordsets2words.pl" ./wordsets2words.pl
	ls -lL wordsets2words.pl

xmlg : # my own version: build/copyfrom my guidod homedir parts
	$(MAKES) xmlgconfigure "USRLOCAL=$(MY_USRLOCAL)" "XMLG=$(MY_XMLG)" \
	"EMUL_PFE=$(MY_EMUL_PFE)"
	$(MAKES) xmlgclean "XMLG=$(MY_XMLG)"
	$(MAKES) xmlgmake  "XMLG=$(MY_XMLG)"
	$(MAKES) xmlgcopy  "XMLG=$(MY_XMLG)"
xmlgg : xmlg
	$(MAKES) xmlgcopy2 "XMLG=$(MY_XMLG)"
xmlggcopy :
	$(MAKES) xmlgcopy xmlgcopy2 "XMLG=$(MY_XMLG)"
diff :
	$(MAKES) $(WORDS).xml
	diff -u $(WORDS).xml $(WORDS)
