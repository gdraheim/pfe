## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = 1.4 foreign dist-bzip2
ACLOCAL_AMFLAGS = -I ../uses
WANT_AUTOMAKE = 1.7
WANT_AUTOCONF = 2.57


OPTIM = @WITH_OPT@    @OPTIM@ @WITHDEF_REGS@
DEBUG = @WITHOUT_OPT@ @DEBUG@
# OPT_FIG= @WITH_FIG@ @WITHDEF_FIG@ # now in config.h 

SRC_K12= @WITH_K12@ term-k12.c main-k12.c
OPT_K12= @WITH_K12@ -DSTATIC_MAIN
HDR_K12= @WITH_K12@ term-k12.h main-k12.h

SRC_FLOAT = floating-ext.c
OPT_FLOAT = @WITHOUT_FLOAT@ -DP4_NO_FP -msoft-float
SRC_TERM = term-def.c # term-lib.c   #  FIXME: @SRC_TERM@
LIB_TERM = @LIB_TERM@
PFE_DEFS = @WITHDEF_NO_COMPLETION@ @WITHDEF_FIG@
GCC_WARN = @GCC_WARN@
WITH_CFLAGS = @WITH_CFLAGS@
USER_CONFIG = @USER_CONFIG@

SRC_DPANS = core-sub.c core-ext.c block-sub.c block-ext.c dict-sub.c \
		double-sub.c double-ext.c exception-sub.c exception-ext.c \
		engine-sub.c facility-ext.c file-ext.c file-sub.c \
		locals-ext.c memory-alloc-ext.c pfe-words.c \
		tools-ext.c search-order-ext.c string-ext.c \
		main-sub.c memory-sub.c $(SRC_FLOAT)
SRC_EXTRA = debug-ext.c forth-83-ext.c forth-usual-ext.c lined.c \
		misc-ext.c posix-ext.c shell-os-ext.c cdecl-ext.c \
		dlfcn-sub.c dlfcn-ext.c environ-ext.c signals-ext.c \
		system-ext.c utils-sub.c useful-ext.c your-ext.c \
		term-sub.c term-ext.c $(SRC_TERM) \
		version-sub.c
SRC_CONTRIB = gforth-ext.c dstrings-ext.c

EXTRA_pfe_SOURCES = main-k12.c \
	term-curses.c term-dj.c term-emx.c term-k12.c term-wat.c term-lib.c

noinst_pfe_headers = _missing.h _nonansi.h
pkginc_pfe_headers = \
	config.h def-comp.h def-config.h def-const.h def-macro.h \
	def-paths.h def-regs.h def-types.h def-words.h def-xtra.h \
	incl-ext.h incl-sub.h incl-sup.h pfe-base.h \
	lined.h logging.h pfe.h term-sub.h $(HDR_K12) \
	dstrings-ext.h 
inc_pfe_headers = pfe-config.h pfe-target.h
# the generated headers...
pkginc_gen_headers = 	dlfcn-sub.h double-sub.h main-sub.h engine-sub.h \
	block-sub.h block-ext.h cdecl-ext.h \
	core-ext.h debug-ext.h double-ext.h \
	edit-ext.h exception-sub.h exception-ext.h environ-ext.h \
	facility-ext.h file-ext.h floating-ext.h \
	forth-83-ext.h forth-usual-ext.h locals-ext.h \
	memory-alloc-ext.h misc-ext.h posix-ext.h search-order-ext.h \
	shell-os-ext.h signals-ext.h string-ext.h struct-ext.h \
	system-ext.h tools-ext.h term-ext.h \
	useful-ext.h your-ext.h \
	core-sub.h dict-sub.h file-sub.h memory-sub.h utils-sub.h \
	version-sub.h \
	gforth-ext.h

lib_LTLIBRARIES = libpfe.la
libpfe_la_SOURCES = $(SRC_DPANS) $(SRC_EXTRA) $(SRC_CONTRIB)
libpfe_la_LDFLAGS = -export-dynamic -module -release $(VERSION)
libpfe_la_LIBADD = @libpfe_LIBADD@

CCOPTS= @cc_opt_pipe@ \
 @cc_opt_char_subscripts@ \
 @cc_opt_write_strings@ \
 @cc_opt_strict_prototypes@ 
INCLUDES= $(OPT_FLOAT) $(OPTIM) $(DEBUG) $(OPT_FIG) $(OPT_K12) \
 $(CCOPTS) $(OPT_EDIT) $(GCC_WARN) $(WITH_CFLAGS) $(USER_CONFIG)
DEFS= @DEFS@ -I. -I$(srcdir)/..            # do not do -I$(srcdir) !!

bin_PROGRAMS = pfe
pfe_SOURCES = main-stdc.c
pfe_LDADD = libpfe.la $(LIB_TERM)  @pfe_LDADD@
pfe_LDFLAGS = -export-dynamic


MODULELIST = edit.la toolbelt.la struct.la structs.la module.la
pkglib_LTLIBRARIES = @WITH_MODULES@ $(MODULELIST)
EXTRA_LTLIBRARIES = lib-dummy-with-modules.la
pkginclude_HEADERS = $(pkginc_pfe_headers) $(pkginc_gen_headers)
include_HEADERS = $(inc_pfe_headers)

edit_la_SOURCES = edit-ext.c
edit_la_LDFLAGS = -export-dynamic -module -avoid-version
edit_la_LIBADD = @libpfe_LIBADD_MODULE@            # libpfe.la
edit_la_DEFS    = -DMODULE # does not work...
edit-ext.lo : edit-ext.c
	$(LTCOMPILE) -DMODULE -c $<

toolbelt_la_SOURCES = toolbelt-ext.c
toolbelt_la_LDFLAGS = -export-dynamic -module -avoid-version
toolbelt_la_LIBADD = @libpfe_LIBADD_MODULE@
toolbelt_la_DEFS    = -DMODULE # does not work...
toolbelt-ext.lo : toolbelt-ext.c
	$(LTCOMPILE) -DMODULE -c $<

struct_la_SOURCES = struct-ext.c
struct_la_LDFLAGS = -export-dynamic -module -avoid-version
struct_la_LIBADD = @libpfe_LIBADD_MODULE@
struct_la_DEFS    = -DMODULE # does not work...
struct-ext.lo : struct-ext.c
	$(LTCOMPILE) -DMODULE -c $<

structs_la_SOURCES = structs-ext.c
structs_la_LDFLAGS = -export-dynamic -module -avoid-version
structs_la_LIBADD = @libpfe_LIBADD_MODULE@ struct.la
structs_la_DEFS    = -DMODULE # does not work...
structs-ext.lo : structs-ext.c
	$(LTCOMPILE) -DMODULE -c $<

module_la_SOURCES = module-ext.c
module_la_LDFLAGS = -export-dynamic -module -avoid-version
module_la_LIBADD = @libpfe_LIBADD_MODULE@
module_la_DEFS    = -DMODULE # does not work...
module-ext.lo : module-ext.c
	$(LTCOMPILE) -DMODULE -c $<


EXTRA_HEADERS=
e = @ac_exeext@
# --------------------------------------------------------------------- #

am: am-Makefile.in am-configure
	automake --foreign

clean-am: 
	rm -f Makefile config.status config.log

am-Makefile.in: Makefile.am
	automake --foreign

am-configure: configure.in 
	aclocal && autoconf && autoheader

@MAINTAINER_MODE_FALSE@ $(OBJECTS) : $(srcdir)/incl-gen.h
$(pkginc_gen_headers) : $(srcdir)/incl-gen.h

      # if perl is not installed, the old files are left untouched.
$(srcdir)/incl-gen.h : $(pkginc_gen_headers:.h=.c)
	@ for F in $? "" ; do if test ! -z "$$F" ; then : \
	; perl $(srcdir)/../mk/Make-H.pl $$F \
	; fi done 
	@ echo creating $@ \
	; echo '/*' `date` '*/' >$@ \
	; for F in $(pkginc_gen_headers) ; do : \
	; echo '#include <pfe/'$$F'>' >>$@ \
	; done
dep-h : $(srcdir)/incl-gen.h

distclean-genheaders:
	cd $(srcdir) && rm -f $(pkginc_gen_headers)

config:
	rm -f config.cache
	@configurecall@

reconfigure:
	@configurecall@

all-local: man
install-data-local : install-man install-scripts
install-exec-local : install-generic-config

DOCDIR= $(srcdir)/../doc
TESTDIR= $(srcdir)/../test
LIB4DIR= $(srcdir)/../lib

install-doc doc install-docs docs :
	$(MAKE) -C $(DOCDIR) prefix=$(prefix) $@ || true
pfe.1 : pfe$(EXEEXT)
	perl $(DOCDIR)/help2man.pl ./pfe$(EXEEXT) > $@ || true
	@ test -f $@ || head $(srcdir)/../pfe.spec > $@
man : pfe.1
install-man : pfe.1
	$(mkinstalldirs) $(DESTDIR)$(mandir)/man1
	$(INSTALL_DATA) pfe.1 $(DESTDIR)$(mandir)/man1
install-scripts : 
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) $(TESTDIR)/*.fs  $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) $(TESTDIR)/*.4th $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) $(LIB4DIR)/*.4th $(DESTDIR)$(pkgdatadir)
install-generic-config:
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	$(INSTALL_SCRIPT) @GENERIC_CONFIG@ $(DESTDIR)$(bindir)

# check-tests will do some minor sanity checks. There may be something awefully
# wrong if these fail. check-local is also called by "make check" or distcheck
check-local:  
	test -d $(TESTDIR)
	@ OK="" ; BAD="" \
	; for f in $(TESTDIR)/*.4th \
	; do : \
	;  check=`basename $$f .4th` \
	;  if test -r "$(TESTDIR)/$$check.sh" \
	;  then echo $$check .... \
	;   ./pfe --bye --quiet -I $(TESTDIR) $$f >$$check.out \
	;   if $(SHELL) "$(TESTDIR)/$$check.sh" $$check.out \
	;   then OK="$$OK $$check" ; else BAD="$$BAD $$check" \
	;   fi \
	;  fi \
	; done \
	; test -z "$$OK"  || echo "OK : $$OK" \
	; test -z "$$BAD" || echo "BAD : $$BAD" \
	; true # test -z "$$BAD" # let the check fail

# tests mean to check if the latest changes to the package made for
# some kind of difference in behaviour with respect to some checks.
# if this target is run for the first time (virgin call), it will 
# simply create local reference output, i.e. it will always succeed 
DIFFFLAGS=-bB
tests: 
	test -d $(TESTDIR)
	@ OK="" ; BAD="" \
	; for f in $(TESTDIR)/*.4th \
	; do : \
	;  check=`basename $$f .4th` \
	;  if test -r "$(TESTDIR)/$$check.chk" \
	;  then echo $$check .... \
	;   ./pfe --bye --quiet -I $(TESTDIR) $$f >$$check.out \
	;   test -f $$check.our || cp $$check.out $$check.our \
	;   diffs=`diff $(DIFFFLAGS) $$check.our $$check.out | wc -l` \
	;   if test "$$diffs" -ne "0" \
	;   then BAD="$$BAD $$check" \
	;    echo $$check: " ... HAS $$diffs DIFFERENCES!!!" \
	;   else OK="$$OK $$check" \
	;   fi \
	;  fi \
	; done \
	; test -z "$$OK"  || echo "OK : $$OK" \
	; test -z "$$BAD" || echo "BAD : $$BAD" \
	; test -z "$$BAD" # let the check fail

clean-test:
	-rm -f *.out *.our

# ---------------------------------------------------------------------- #

%.i: %.c
	@echo '$(COMPILE) -E $<'; \
	$(COMPILE) -E $< >$@

