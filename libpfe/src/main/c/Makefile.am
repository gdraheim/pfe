AUTOMAKE_OPTIONS = 1.6 foreign no-dist-gzip dist-bzip2 dist-lzma
AUTOTOOL_VERSION = autoconf-2.52 automake-1.5 libtool-1.4.2
ACLOCAL_AMFLAGS = -I ../../../../config/m4
# $Id: Makefile.am,v 1.9 2008-12-22 09:37:22 guidod Exp $

OPTIM = @WITH_OPT@    @OPTIM@
DEBUG = @WITHOUT_OPT@ @DEBUG@

OPT_FLOAT = @WITHOUT_FLOAT@ -DP4_NO_FP -msoft-float
SRC_TERM = term-def.c
LIB_TERM = @LIB_TERM@
GCC_WARN = @GCC_WARN@
WITH_CFLAGS = @WITH_CFLAGS@
USER_CONFIG = @USER_CONFIG@
PFE_DEFS = @PFE_DEFS@

pfe_sources = def-check.c \
	core-sub.c core-ext.c core-mix.c \
	block-sub.c block-ext.c block-mix.c \
	cdecl-ext.c dict-sub.c dict-comp.c \
	header-sub.c header-ext.c \
	double-sub.c double-ext.c double-mix.c \
	exception-sub.c exception-ext.c \
	engine-sub.c engine-ext.c engine-set.c \
	environ-ext.c option-ext.c option-set.c \
        facility-ext.c facility-mix.c \
	file-ext.c file-sub.c file-mix.c \
	locals-ext.c memory-alloc-ext.c \
	tools-sub.c tools-ext.c tools-mix.c \
	search-order-ext.c string-ext.c \
	memory-sub.c chainlist-ext.c debug-ext.c lined.c \
	forth-83-ext.c forth-usual-ext.c misc-ext.c posix-ext.c \
	shell-os-ext.c  signals-ext.c \
	system-ext.c useful-ext.c your-ext.c \
	term-sub.c term-ext.c $(SRC_TERM) \
        dl-def.c dl-ext.c \
	with-spy.c version-sub.c \
	_missing.c os-string.c os-ctype.c os-delay.c \
	p4-gettimeofday.c

# these two get compiled by dl-internal.c when appropriate
## SRC_CONTRIB = gforth-ext.c dstrings-ext.c ## does not work in automake 1.9

EXTRA_pfe_SOURCES = $(SRC_CONTRIB) \
	main-stdc.c main-mmap.c main-static.c main-alloc.c    main-sub.c \
	term-curses.c term-dj.c term-emx.c term-wat.c term-lib.c \
	dl-hpux.c dl-none.c dl-vxworks.c dl-dlfcn.c dl-win32.c \
	lib-sdl.c chain-ext.c term-wincon.c term-x11.c 


INCLUDES= $(OPT_FLOAT) $(OPTIM) $(DEBUG) $(PFE_DEFS) $(GCC_WARN) 
DEFS= @DEFS@ -I. -I../include -I$(srcdir)/../include   
 # do not do -I$(srcdir) !!
 pfeincludedir=$(srcdir)/../include

bin_PROGRAMS = pfe
pfe_SOURCES = main-def.c
pfe_LDADD = libpfe.la @WITHOUT_MODULES@ libpfemodule.la
pfe_LDFLAGS = -export-dynamic

lib_LTLIBRARIES = libpfe.la @WITHOUT_MODULES@ libpfemodule.la
libpfe_la_SOURCES = $(pfe_sources)
libpfe_la_LDFLAGS = -export-dynamic @libpfe_VERSION@ 
libpfe_la_LIBADD = @DLLFLAGS@ $(LIB_TERM)

libpfemodule_la_LDFLAGS = -export-dynamic @libpfe_VERSION@
libpfemodule_la_LIBADD = @DLLFLAGS@ @libpfe_la@
libpfemodule_la_SOURCES = dl-internal.c \
	edit-ext.c toolbelt-ext.c gforth-ext.c \
        struct-ext.c structs-ext.c module-ext.c dstrings-ext.c \
	floating-ext.c floating-mix.c help-ext.c zchar-ext.c \
	fpnostack-ext.c complex-ext.c assembler-ext.c

MODULELIST = edit.la toolbelt.la gforth.la zchar.la help.la \
             struct.la module.la dstrings.la smart-go.la termcatch.la \
             floating.la fpnostack.la complex.la structs.la \
             assembler.la
pfelibdir = ${libdir}/@PACKAGE@@VARIANT@
pfelib_LTLIBRARIES = @WITH_MODULES@ $(MODULELIST)
EXTRA_LTLIBRARIES = lib-dummy-with-modules.la
lib_dummy_with_modules_la_SOURCES = 
pkginclude_HEADERS = $(pkginc_pfe_headers) $(pkginc_gen_headers)
pkginclude_DATA = $(config_pfe_headers)

edit_la_SOURCES = edit-dll.c edit-ext.c
edit_la_LDFLAGS = -export-dynamic -module -avoid-version
edit_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/edit-dll.c : $(pfeincludedir)/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/edit/ "$?" >> $@

toolbelt_la_SOURCES =  toolbelt-dll.c toolbelt-ext.c
toolbelt_la_LDFLAGS = -export-dynamic -module -avoid-version
toolbelt_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/toolbelt-dll.c : $(pfeincludedir)/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/toolbelt/ "$?" >> $@

gforth_la_SOURCES = gforth-dll.c gforth-ext.c
gforth_la_LDFLAGS = -export-dynamic -module -avoid-version
gforth_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/gforth-dll.c : $(pfeincludedir)/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/gforth/ "$?" >> $@

struct_la_SOURCES = struct-dll.c struct-ext.c
struct_la_LDFLAGS = -export-dynamic -module -avoid-version
struct_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/struct-dll.c : $(pfeincludedir)/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/struct/ "$?" >> $@

structs_la_SOURCES = structs-dll.c structs-ext.c 
structs_la_LDFLAGS = -export-dynamic -module -avoid-version
structs_la_LIBADD = @DYNALINK@ -Lrenamed struct.la @libpfe_la@
$(srcdir)/structs-dll.c : $(pfeincludedir)/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/structs/ "$?" >> $@
structs_la_DEPENDENCIES = renamed/struct.la
renamed/struct.la : struct.la
	$(MAKE) lib-renamed

dstrings_la_SOURCES =  dstrings-dll.c dstrings-ext.c
dstrings_la_LDFLAGS = -export-dynamic -module -avoid-version
dstrings_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/dstrings-dll.c : $(pfeincludedir)/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/dstrings/ "$?" >> $@

floating_la_SOURCES = floating-dll.c floating-mix.c floating-ext.c
floating_la_LDFLAGS = -export-dynamic -module -avoid-version
floating_la_LIBADD = -lm @DYNALINK@ @libpfe_la@
$(srcdir)/floating-dll.c : $(pfeincludedir)/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/floating/ "$?" >> $@

fpnostack_la_SOURCES = fpnostack-dll.c fpnostack-ext.c
fpnostack_la_LDFLAGS = -export-dynamic -module -avoid-version
fpnostack_la_LIBADD = -lm @DYNALINK@ @libpfe_la@
$(srcdir)/fpnostack-dll.c : $(pfeincludedir)/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/fpnostack/ "$?" >> $@

complex_la_SOURCES = complex-dll.c complex-ext.c
complex_la_LDFLAGS = -export-dynamic -module -avoid-version
complex_la_LIBADD =  -lm @DYNALINK@ -Lrenamed floating.la @libpfe_la@
$(srcdir)/complex-dll.c : $(pfeincludedir)/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/complex/ "$?" >> $@
complex_la_DEPENDENCIES = renamed/floating.la
renamed/floating.la : floating.la
	$(MAKE) lib-renamed

smart_go_la_SOURCES = smart-go-dll.c smart-go-ext.c
smart_go_la_LDFLAGS = -export-dynamic -module -avoid-version
smart_go_la_LIBADD = @DYNALINK@ @libpfe_la@ 
$(srcdir)/smart-go-dll.c : $(pfeincludedir)/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/smart_go/ "$?" >> $@

help_la_SOURCES = help-dll.c help-ext.c
help_la_LDFLAGS = -export-dynamic -module -avoid-version
help_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/help-dll.c : $(pfeincludedir)/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/help/ "$?" >> $@

termcatch_la_SOURCES = termcatch-dll.c termcatch-ext.c
termcatch_la_LDFLAGS = -export-dynamic -module -avoid-version
termcatch_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/termcatch-dll.c : $(pfeincludedir)/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/termcatch/ "$?" >> $@

assembler_la_SOURCES = assembler-dll.c assembler-ext.c
assembler_la_LDFLAGS = -export-dynamic -module -avoid-version
assembler_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/assembler-dll.c : $(pfeincludedir)/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/assembler/ "$?" >> $@

zchar_la_SOURCES = zchar-dll.c zchar-ext.c
zchar_la_LDFLAGS = -export-dynamic -module -avoid-version
zchar_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/zchar-dll.c : $(pfeincludedir)/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/zchar/ "$?" >> $@

module_la_SOURCES =  module-dll.c module-ext.c
module_la_LDFLAGS = -export-dynamic -module -avoid-version
module_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/module-dll.c : $(pfeincludedir)/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	cat "$?" >> $@ 
# this is the original module-dll

EXTRA_HEADERS=

# --------------------------------------------------------------------- #


# libtool 1.4.3 /gcc'ld combo is too stupid to link modules correctly
  install_pfelibLTLIBRARIES  = install-pfelibLTLIBRARIES
$(install_pfelibLTLIBRARIES) : install-renamed
install-renamed : $(MODULELIST)
	- $(mkinstalldirs) $(DESTDIR)$(pfelibdir)/renamed
	@ for lib in $(MODULELIST) \
	; do name=`grep "^dlname=" $$lib | sed -e 's,dlname=,,' -e "s,',,g"` \
	; cp $$lib $(DESTDIR)$(pfelibdir)/$$name \
	; echo "ln -sf $(DESTDIR)$(pfelibdir)/$$name renamed/lib$$name" \
	; (cd $(DESTDIR)$(pfelibdir)/renamed && ln -sf ../$$name lib$$name) \
	; done ; true

$(MODULELIST) : renamed.dir
renamed.dir : 
	test -d renamed || mkdir renamed
	date > $@
lib-renamed : renamed.dir
	@ for lib in $(MODULELIST) \
	; do test -f $$lib || continue \
	; test ! -f renamed/$$lib || continue \
	; echo cp $$lib renamed/ ; cp $$lib renamed/ \
	; name=`grep "^dlname=" $$lib | sed -e 's,dlname=,,' -e "s,',,g"` \
	; test -h renamed/lib$$name && continue \
	; _libs=".libs" ; test -d _libs && _libs="_libs" \
	; echo "ln -sf $$_libs/$$name renamed/lib$$name" \
	; test -f $$_libs/$$name || echo "MISSING $$_libs/$$name ($$lib)" \
	; (cd renamed && ln -sf ../$$_libs/$$name lib$$name) \
	; done ; true

CLEANFILES = renamed.dir renamed/*
# ---------------------------------------------------------------------- #

## config.status : config.status.test

## config.status.test :
##	test -f config.status || $(SHELL) $(configurecall)

%.i: %.c
	@echo '$(COMPILE) -E $<'; \
	$(COMPILE) -E $< >$@

%.s: %.c
	@echo '$(COMPILE) -E $<'; \
	$(COMPILE) -S $< >$@

