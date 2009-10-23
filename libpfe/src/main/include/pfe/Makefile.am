AUTOMAKE_OPTIONS = 1.6 foreign no-dist-gzip dist-bzip2 dist-lzma
AUTOTOOL_VERSION = autoconf-2.52 automake-1.5 libtool-1.4.2
ACLOCAL_AMFLAGS = -I ../../../config/m4

EXTRA_DIST = module-dll.inc \
             $(noinst_pfe_headers) \
             $(pkginc_pfe_headers) \
             $(pkginc_gen_headers) \
             $(NULL)
             
pkginclude_HEADERS = module-dll.inc \
                     $(pkginc_pfe_headers) \
                     $(pkginc_gen_headers)  

noinst_pfe_headers = _missing.h _nonansi.h \
                    def-comp-c89.h 
pkginc_pfe_headers = \
        os-ctype.h os-string.h os-setjmp.h os-fesetenv.h os-delay.h \
        def-cell.h def-comp.h def-config.h def-const.h \
        def-gcc.h def-macro.h def-paths.h \
	def-regmacro.h def-regs.h def-regth.h def-restore.h def-sbrcode.h \
        def-types.h def-words.h \
	def-limits.h def-pth.h \
        incl-ext.h incl-sub.h incl-sup.h pfe-base.h \
	lined.h logging.h pfe.h term-sub.h         incl-mix.h \
	dstrings-ext.h                                        main-sub.h \
        pfe-ext.h pfe-sub.h pfe-sup.h pfe-mix.h pfe-set.h
config_pfe_headers = config.h _config.h _target.h$(noinst_pfe_headers)
# the generated headers...
pkginc_gen_headers = double-sub.h \
	engine-sub.h engine-set.h option-set.h \
	block-sub.h block-ext.h block-mix.h \
	cdecl-ext.h chainlist-ext.h                 chain-ext.h \
	core-sub.h core-ext.h core-mix.h \
	double-ext.h double-mix.h \
	debug-ext.h dict-sub.h dict-comp.h \
	exception-sub.h exception-ext.h \
	environ-ext.h facility-ext.h facility-mix.h \
	file-ext.h file-mix.h file-sub.h \
	forth-83-ext.h forth-usual-ext.h \
	header-sub.h header-ext.h \
	locals-ext.h memory-alloc-ext.h \
	misc-ext.h option-ext.h \
	posix-ext.h search-order-ext.h \
	shell-os-ext.h signals-ext.h \
	string-ext.h system-ext.h \
	tools-sub.h tools-ext.h tools-mix.h term-ext.h \
	useful-ext.h your-ext.h \
	memory-sub.h version-sub.h \
	floating-ext.h floating-mix.h \
	fpnostack-ext.h complex-ext.h \
	edit-ext.h help-ext.h gforth-ext.h \
        assembler-ext.h \

# -------------------------------------------------------------------------


# config.h : incl-gen.t

TOOL_PFE_DIR = $(srcdir)/../../../tool/pfe
SRC_PFE_DIR = $(srcdir)/../../../main/c
INC_PFE_DIR = $(srcdir)

Makedeps.new.mk : Makefile 
	{ for header in $(pkginc_gen_headers); do : \
	; file=`basename $$header .h` \
	; if test -s $(SRC_PFE_DIR)/$$file.c; then : \
	; A="$$file.h" \
	; B="../../c/$$file.c" \
	; C="../../../tool/pfe/make-header.py" \
	; echo "$$A : $$B ; $$C \$$< \$$@" \
	; fi ; done ; } > $@
	@ O=$(srcdir)/Makedeps.mk; if cmp -s $@ $$O; then : \
	; echo ": no update of $$O required (same content as $@)" \ 
	; else echo "cp $@ $$O" ; cp $@ $$O ; fi
	
gendeps : Makedeps.new.mk

-include Makedeps.mk

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

headers:
	$(MAKE) $(pkg_inc_gen_headers)

CLEANFILES = *.tmp *.tmp.*
DISTCLEANFILES = *.new *.new.*
