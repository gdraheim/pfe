VPATH = ../../pfe
these: index.html all-fcode.html all-wordsets.html

include ../../pfe/Makefile.am
SRC_FLOAT = floating-ext.c
SRC_TERM = term-def.c
SRC_ALL = \
	$(libpfe_la_SOURCES) \
	$(EXTRA_pfe_SOURCES) \
	$(pfe_SOURCES) \
	$(edit_la_SOURCES) \
	$(toolbelt_la_SOURCES) \
	$(struct_la_SOURCES) \
	$(structs_la_SOURCES) \
	$(module_la_SOURCES) \

index.html : $(SRC_ALL:.c=.html)
	@ echo '<html>' >$@
	@ test ! -f index.header.in || cat index.header.in >>$@
	@ echo '<table border=0 cellpadding=20><tr valign=top><td><big>' >>$@
	@ for o in $(SRC_ALL:.c=.html) \
	; do : \
	; echo '<br /><a href="'$$o'">'`basename $$o .html`'</a>' \
	>>$@ \
	; if test "$$o" = "your-ext.html" \
	; then echo '</big></td><td><big>' >>$@ \
	; fi \
	; if test "$$o" = "floating-ext.html" \
	; then echo '</big></td><td><big>' >>$@ \
	; fi \
	; done ; true
	@ echo '</big></td></tr></table><hr /><pre>' >>$@
	@ test ! -f ../../pfe.spec || sed -e '/%prep/q' ../../pfe.spec >>$@
	@ echo '</pre><hr /> generated - ' `date` >>$@
	@ echo '</html>' >>$@
	@ echo $@

# ---------- fcode -----------

%.fcode : %.xml
	@ xm-perl xm::pfe::cutfcodedocs $< >$@

all-fcode.html : all-fcode.xml

all-fcode-cuts.xml : $(SRC_ALL:.c=.xml)
	@ xm-perl xm::pfe::cutfcodedocs $(SRC_ALL:.c=.xml) >$@
all-fcode.xml : all-fcode-cuts.xml
	@ xm-perl xm::pfe::fcodesortnspill $< >$@

# ------- wordsets -------------

%.wordset : %.xml
	@ xm-perl xm::pfe::cutwordsetdocs $< >$@

all-wordsets.xml : $(SRC_ALL:.c=.wordset)
	@ echo '<ALLWORDSETS>' >$@
	@ cat $(SRC_ALL:.c=.wordset) >>$@ 
	@ echo '</ALLWORDSETS>' >>$@
	@ test -s $@ && echo $@

# ---------- rules -----------

clean: 
	rm -f *.html *.xml

%.xml : ../../pfe/%.c
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

%.html : %.xml
	@ echo $<:html
	@ xm-perl xm::css::mkhtml4 -- --ext=xml --css=PFE.css $< 



