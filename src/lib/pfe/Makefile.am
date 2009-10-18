AUTOMAKE_OPTIONS = 1.5 foreign dist-bzip2
AUTOTOOL_VERSION = autoconf-2.52 automake-1.5 libtool-1.4.2
ACLOCAL_AMFLAGS = -I ../uses
# $Id: Makefile.am,v 1.9 2008-12-22 09:37:22 guidod Exp $

OPTIM = @WITH_OPT@    @OPTIM@
DEBUG = @WITHOUT_OPT@ @DEBUG@

## SRC_K12= ## @WITH_K12@ term-k12.c main-k12.c
## OPT_K12= ## @WITH_K12@ -DSTATIC_MAIN
## HDR_K12= ## @WITH_K12@ term-k12.h main-k12.h

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
	_missing.c os-string.c os-ctype.c os-delay.c os-fesetenv.h \
	p4-gettimeofday.c

# these two get compiled by dl-internal.c when appropriate
## SRC_CONTRIB = gforth-ext.c dstrings-ext.c ## does not work in automake 1.9

EXTRA_pfe_SOURCES = $(SRC_CONTRIB) \
	main-stdc.c main-mmap.c main-static.c main-alloc.c    main-sub.c \
	main-k12.c emu-menu-table.c \
	term-curses.c term-dj.c term-emx.c term-k12.c term-wat.c term-lib.c \
	dl-hpux.c dl-none.c dl-vxworks.c dl-dlfcn.c dl-win32.c \
	host-k12.c lib-sdl.c chain-ext.c term-wincon.c term-x11.c 

noinst_pfe_headers = _missing.h _nonansi.h \
                    def-comp-c89.h main-k12.h term-k12.h 
pkginc_pfe_headers = \
        os-ctype.h os-string.h os-setjmp.h os-delay.h p4-gettimeofday.h \
        def-cell.h def-comp.h def-config.h def-const.h \
        def-gcc.h def-macro.h def-paths.h \
	def-regmacro.h def-regs.h def-regth.h def-restore.h def-sbrcode.h \
        def-types.h def-words.h \
	def-limits.h def-pth.h \
        incl-ext.h incl-sub.h incl-sup.h pfe-base.h \
	lined.h logging.h pfe.h term-sub.h         incl-mix.h \
	dstrings-ext.h                                        main-sub.h \
        pfe-ext.h pfe-sub.h pfe-sup.h pfe-mix.h pfe-set.h
config_pfe_headers = config.h _config.h _target.h
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
	struct-ext.h edit-ext.h \
	help-ext.h stackhelp-ext.h gforth-ext.h \
        assembler-ext.h \
	smart-go-ext.h stackhelp-ext.h
# note: on k12xx, we link dl-internal directly since we also link main-k12
# but the dl-internal knows only zchar-ext, the other modules are external.

INCLUDES= $(OPT_FLOAT) $(OPTIM) $(DEBUG) $(PFE_DEFS) $(GCC_WARN) 
DEFS= @DEFS@ -I. -I.. -I$(srcdir)/..            # do not do -I$(srcdir) !!

EXTRA_DIST = $(noinst_pfe_headers) complex-pfe.log

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
	fpnostack-ext.c complex-ext.c assembler-ext.c # no stackhelp so far

MODULELIST = edit.la toolbelt.la gforth.la zchar.la help.la stackhelp.la \
             struct.la module.la dstrings.la smart-go.la termcatch.la \
             socket.la floating.la fpnostack.la complex.la structs.la \
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
$(srcdir)/edit-dll.c : $(srcdir)/module-dll.c
	sed s/module/edit/ "$?" > $@

toolbelt_la_SOURCES =  toolbelt-dll.c toolbelt-ext.c
toolbelt_la_LDFLAGS = -export-dynamic -module -avoid-version
toolbelt_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/toolbelt-dll.c : $(srcdir)/module-dll.c
	sed s/module/toolbelt/ "$?" > $@

gforth_la_SOURCES = gforth-dll.c gforth-ext.c
gforth_la_LDFLAGS = -export-dynamic -module -avoid-version
gforth_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/gforth-dll.c : $(srcdir)/module-dll.c
	sed s/module/gforth/ "$?" > $@

struct_la_SOURCES = struct-dll.c struct-ext.c
struct_la_LDFLAGS = -export-dynamic -module -avoid-version
struct_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/struct-dll.c : $(srcdir)/module-dll.c
	sed s/module/struct/ "$?" > $@

structs_la_SOURCES = structs-dll.c structs-ext.c 
structs_la_LDFLAGS = -export-dynamic -module -avoid-version
structs_la_LIBADD = @DYNALINK@ -Lrenamed struct.la @libpfe_la@
$(srcdir)/structs-dll.c : $(srcdir)/module-dll.c
	sed s/module/structs/ "$?" > $@
structs_la_DEPENDENCIES = renamed/struct.la
renamed/struct.la : struct.la
	$(MAKE) lib-renamed

dstrings_la_SOURCES =  dstrings-dll.c dstrings-ext.c
dstrings_la_LDFLAGS = -export-dynamic -module -avoid-version
dstrings_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/dstrings-dll.c : $(srcdir)/module-dll.c
	sed s/module/dstrings/ "$?" > $@

floating_la_SOURCES = floating-dll.c floating-mix.c floating-ext.c
floating_la_LDFLAGS = -export-dynamic -module -avoid-version
floating_la_LIBADD = -lm @DYNALINK@ @libpfe_la@
$(srcdir)/floating-dll.c : $(srcdir)/module-dll.c
	sed s/module/floating/ "$?" > $@

fpnostack_la_SOURCES = fpnostack-dll.c fpnostack-ext.c
fpnostack_la_LDFLAGS = -export-dynamic -module -avoid-version
fpnostack_la_LIBADD = -lm @DYNALINK@ @libpfe_la@
$(srcdir)/fpnostack-dll.c : $(srcdir)/module-dll.c
	sed s/module/fpnostack/ "$?" > $@

complex_la_SOURCES = complex-dll.c complex-ext.c
complex_la_LDFLAGS = -export-dynamic -module -avoid-version
complex_la_LIBADD =  -lm @DYNALINK@ -Lrenamed floating.la @libpfe_la@
$(srcdir)/complex-dll.c : $(srcdir)/module-dll.c
	sed s/module/complex/ "$?" > $@
complex_la_DEPENDENCIES = renamed/floating.la
renamed/floating.la : floating.la
	$(MAKE) lib-renamed

smart_go_la_SOURCES = smart-go-dll.c smart-go-ext.c
smart_go_la_LDFLAGS = -export-dynamic -module -avoid-version
smart_go_la_LIBADD = @DYNALINK@ @libpfe_la@ 
$(srcdir)/smart-go-dll.c : $(srcdir)/module-dll.c
	sed s/module/smart_go/ "$?" > $@

help_la_SOURCES = help-dll.c help-ext.c
help_la_LDFLAGS = -export-dynamic -module -avoid-version
help_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/help-dll.c : $(srcdir)/module-dll.c
	sed s/module/help/ "$?" > $@

termcatch_la_SOURCES = termcatch-dll.c termcatch-ext.c
termcatch_la_LDFLAGS = -export-dynamic -module -avoid-version
termcatch_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/termcatch-dll.c : $(srcdir)/module-dll.c
	sed s/module/termcatch/ "$?" > $@

socket_la_SOURCES = socket-dll.c socket-ext.c
socket_la_LDFLAGS = -export-dynamic -module -avoid-version
socket_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/socket-dll.c : $(srcdir)/module-dll.c
	sed s/module/socket/ "$?" > $@

assembler_la_SOURCES = assembler-dll.c assembler-ext.c
assembler_la_LDFLAGS = -export-dynamic -module -avoid-version
assembler_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/assembler-dll.c : $(srcdir)/module-dll.c
	sed s/module/assembler/ "$?" > $@

stackhelp_la_SOURCES = stackhelp-dll.c stackhelp-ext.c \
    core-stk.c block-stk.c cdecl-stk.c chain-stk.c chainlist-stk.c \
    file-stk.c memory-alloc-stk.c string-stk.c stackhelp-stk.c \
    exception-stk.c facility-stk.c misc-stk.c debug-stk.c forth-83-stk.c
stackhelp_la_LDFLAGS = -export-dynamic -module -avoid-version
stackhelp_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/stackhelp-dll.c : $(srcdir)/module-dll.c
	sed s/module/stackhelp/ "$?" > $@

zchar_la_SOURCES = zchar-dll.c zchar-ext.c
zchar_la_LDFLAGS = -export-dynamic -module -avoid-version
zchar_la_LIBADD = @DYNALINK@ @libpfe_la@
$(srcdir)/zchar-dll.c : $(srcdir)/module-dll.c
	sed s/module/zchar/ "$?" > $@

module_la_SOURCES =  module-dll.c module-ext.c
module_la_LDFLAGS = -export-dynamic -module -avoid-version
module_la_LIBADD = @DYNALINK@ @libpfe_la@
# the original module-dll.c has no source-sed-rule

EXTRA_HEADERS=

pkgconfigdir = ${libdir}/pkgconfig
pkgconfig_DATA = pfe.pc
# --------------------------------------------------------------------- #
auto:
	aclocal
	autoconf
	autoheader
	automake -a -c

it : all 

# config.h : incl-gen.t

incl-gen : incl-gen.t
incl-gen.t : # make sure it is built atleast after unpacking
	$(MAKE) $(srcdir)/incl-gen.h
	cp $(srcdir)/incl-gen.h $@

      # if perl is not installed, the old files are left untouched.
$(srcdir)/incl-gen.h : $(pkginc_gen_headers:.h=.c)
	: perl $(srcdir)/../mk/Make-H.pl $(srcdir)/...
	@ for F in $? "" ; do if test ! -z "$$F" \
	; then   perl $(srcdir)/../mk/Make-H.pl $$F \
	; fi done ; true
	@ echo creating $@ "(master include)" \
	; echo '/*' `date` '*/' >$@ \
	; for F in $(pkginc_gen_headers) ; do : \
	; echo '#include <pfe/'$$F'>' >>$@ \
	; done
gen : $(srcdir)/incl-gen.h
force-gen:
	: perl $(srcdir)/../mk/Make-H.pl $(srcdir)/...
	@ for F in $(pkginc_gen_headers:.h=.c) "" ; do if test ! -z "$$F" \
	; then   perl $(srcdir)/../mk/Make-H.pl $(srcdir)/$$F \
	; fi done ; true

distclean-genheaders:
	cd $(srcdir) && rm -f $(pkginc_gen_headers)

config:
	rm -f config.cache
	@configurecall@

reconfigure:
	@configurecall@

install-data-local : install-scripts install-transform-pkgconfig
install-exec-hook :
	for bin in pfe-exec pfe ; do (cd $(DESTDIR)$(bindir) && \
         ln -sf `echo $$bin | sed '$(transform)'` $$bin); done

SCRIPTDIR= $(srcdir)/..
install-scripts : 
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/test
	$(INSTALL_DATA) $(SCRIPTDIR)/test/*.fs  $(DESTDIR)$(pkgdatadir)/test
	$(INSTALL_DATA) $(SCRIPTDIR)/test/*.4th $(DESTDIR)$(pkgdatadir)/test
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/lib
	$(INSTALL_DATA) $(SCRIPTDIR)/lib/*.4th $(DESTDIR)$(pkgdatadir)/lib
install-transform-pkgconfig:
	$(mkinstalldirs) $(DESTDIR)$(pkgconfigdir)
	@ for f in $(pkgconfig_DATA) ; do test -f $$f || f="$(srcdir)/$$f" \
	; ff=`echo $$f | sed -e '$(transform)'` ; if test "$$f" != "$$ff" \
	; then sed -e "/Name:/$(transform)" $$f > $$ff \
	; if test -s $ff ; then if diff $$f $$ff >/dev/null 2>/dev/null \
	; then echo $(INSTALL_SCRIPT) $$ff $(DESTDIR)$(pkgconfigdir) \
	; $(INSTALL_SCRIPT) $$ff $(DESTDIR)$(pkgconfigdir) ; fi fi fi done
install-it installs : install

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

