BUILD=`uname -msr | tr " /" "__"`.d
ISNOTSRCDIR=test ! -f configure



srcdir = .
top_srcdir = .
pkgdatadir = $(datadir)/pfe
pkglibdir = $(libdir)/pfe
pkgincludedir = $(includedir)/pfe
top_builddir = .
am__cd = CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd
INSTALL = /usr/bin/install -c
install_sh_DATA = $(install_sh) -c -m 644
install_sh_PROGRAM = $(install_sh) -c
install_sh_SCRIPT = $(install_sh) -c
INSTALL_HEADER = $(INSTALL_DATA)
transform = $(program_transform_name)
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
build_triplet = i686-suse-linux
host_triplet = i686-suse-linux
target_triplet = i686-suse-linux
DIST_COMMON = README $(am__configure_deps) $(srcdir)/Makefile.am \
 $(srcdir)/Makefile.in $(top_srcdir)/configure COPYING.LIB \
 INSTALL TODO uses/config.guess uses/config.sub uses/depcomp \
 uses/install-sh uses/ltconfig uses/ltmain.sh uses/missing \
 uses/mkinstalldirs
subdir = .
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
am__aclocal_m4_deps =  \
 $(top_srcdir)/uses/ac_set_default_paths_system.m4 \
 $(top_srcdir)/uses/amversion.m4 $(top_srcdir)/uses/auxdir.m4 \
 $(top_srcdir)/uses/ax_enable_builddir_uname.m4 \
 $(top_srcdir)/uses/ax_spec_file.m4 \
 $(top_srcdir)/uses/ax_spec_package_version.m4 \
 $(top_srcdir)/uses/cond.m4 $(top_srcdir)/uses/init.m4 \
 $(top_srcdir)/uses/install-sh.m4 \
 $(top_srcdir)/uses/lead-dot.m4 \
 $(top_srcdir)/uses/maintainer.m4 $(top_srcdir)/uses/missing.m4 \
 $(top_srcdir)/uses/mkdirp.m4 $(top_srcdir)/uses/options.m4 \
 $(top_srcdir)/uses/sanity.m4 $(top_srcdir)/uses/strip.m4 \
 $(top_srcdir)/uses/tar.m4 $(top_srcdir)/configure.ac
am__configure_deps = $(am__aclocal_m4_deps) $(CONFIGURE_DEPENDENCIES) \
 $(ACLOCAL_M4)
am__CONFIG_DISTCLEAN_FILES = config.status config.cache config.log \
 configure.lineno configure.status.lineno
mkinstalldirs = $(SHELL) $(top_srcdir)/uses/mkinstalldirs
CONFIG_CLEAN_FILES =
SOURCES =
DIST_SOURCES =
RECURSIVE_TARGETS = all-recursive check-recursive dvi-recursive \
 html-recursive info-recursive install-data-recursive \
 install-exec-recursive install-info-recursive \
 install-recursive installcheck-recursive installdirs-recursive \
 pdf-recursive ps-recursive uninstall-info-recursive \
 uninstall-recursive
ETAGS = etags
CTAGS = ctags
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)
distdir = $(PACKAGE)-$(VERSION)
top_distdir = $(distdir)
am__remove_distdir = \
  { test ! -d $(distdir) \
    || { find $(distdir) -type d ! -perm -200 -exec chmod u+w {} ';' \
         && rm -fr $(distdir); }; }
DIST_ARCHIVES = $(distdir).tar.gz $(distdir).tar.bz2
GZIP_ENV = --best
distuninstallcheck_listfiles = find . -type f -print
distcleancheck_listfiles = find . -type f -print
ACLOCAL = ${SHELL} /vol/8/src/cvs/pfe-30/uses/missing --run aclocal-1.9
AMTAR = ${SHELL} /vol/8/src/cvs/pfe-30/uses/missing --run tar
AUTOCONF = ${SHELL} /vol/8/src/cvs/pfe-30/uses/missing --run autoconf
AUTOHEADER = ${SHELL} /vol/8/src/cvs/pfe-30/uses/missing --run autoheader
AUTOMAKE = ${SHELL} /vol/8/src/cvs/pfe-30/uses/missing --run automake-1.9
AWK = gawk
CYGPATH_W = echo
DEFS = -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE=\"pfe\" -DVERSION=\"0.30.98\" 
ECHO_C = 
ECHO_N = -n
ECHO_T = 
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_STRIP_PROGRAM = ${SHELL} $(install_sh) -c -s
LIBOBJS = 
LIBS = 
LTLIBOBJS = 
MAINT = #
MAINTAINER_MODE_FALSE = 
MAINTAINER_MODE_TRUE = #
MAKEINFO = ${SHELL} /vol/8/src/cvs/pfe-30/uses/missing --run makeinfo
PACKAGE = pfe
PACKAGE_BUGREPORT = 
PACKAGE_NAME = pfe
PACKAGE_STRING = 
PACKAGE_TARNAME = 
PACKAGE_VERSION = 0.30.98
PATH_SEPARATOR = :
SET_MAKE = 
SHELL = /bin/sh
STRIP = 
VERSION = 0.30.98
ac_ct_STRIP = 
am__leading_dot = .
am__tar = ${AMTAR} chof - "$$tardir"
am__untar = ${AMTAR} xf -
ax_enable_builddir_sed = sed
bindir = ${exec_prefix}/bin
build = i686-suse-linux
build_alias = 
build_cpu = i686
build_os = linux
build_vendor = suse
datadir = ${prefix}/share
exec_prefix = ${prefix}
host = i686-suse-linux
host_alias = 
host_cpu = i686
host_os = linux
host_vendor = suse
includedir = ${prefix}/include
infodir = ${prefix}/info
install_sh = /vol/8/src/cvs/pfe-30/uses/install-sh
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
localstatedir = ${prefix}/var
mandir = ${datadir}/man
mkdir_p = mkdir -p --
oldincludedir = /usr/include
prefix = /usr/local
program_transform_name = s,x,x,
sbindir = ${exec_prefix}/sbin
sharedstatedir = ${sysconfdir}/default
subdirs =  pfe
sysconfdir = ${prefix}/etc
target = i686-suse-linux
target_alias = 
target_cpu = i686
target_os = linux
target_vendor = suse
AUTOMAKE_OPTIONS = 1.4 foreign dist-bzip2
ACLOCAL_AMFLAGS = -I uses
WANT_AUTOMAKE = 1.7
WANT_AUTOCONF = 2.57
SUBDIRS = pfe
DIST_SUBDIRS = pfe test doc mk lib
all: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

am--refresh: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
$(srcdir)/Makefile.in: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
Makefile: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

$(top_builddir)/config.status: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

$(top_srcdir)/configure: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
$(ACLOCAL_M4): ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
uninstall-info-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

$(RECURSIVE_TARGETS): ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

mostlyclean-recursive clean-recursive distclean-recursive \
maintainer-clean-recursive: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
tags-recursive: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
ctags-recursive: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

ID: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
tags: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

TAGS: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
ctags: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
CTAGS: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

GTAGS: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

distclean-tags: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

distdir: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
dist-gzip: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
dist-bzip2: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

dist-tarZ: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

dist-shar: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

dist-zip: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

dist dist-all: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

distcheck: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
distuninstallcheck: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
distcleancheck: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
check-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
check: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
all-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
installdirs: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
installdirs-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
install: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
install-exec: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
install-data: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
uninstall: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

install-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

installcheck: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
install-strip: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
mostlyclean-generic: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

clean-generic: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

distclean-generic: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

maintainer-clean-generic: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
clean: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

clean-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

distclean: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
distclean-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

dvi: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

dvi-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

html: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

info: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

info-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

install-data-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

install-exec-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

install-info: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

install-man: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

installcheck-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

maintainer-clean: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
maintainer-clean-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

mostlyclean: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

mostlyclean-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

pdf: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

pdf-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

ps: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

ps-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

uninstall-am: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"

uninstall-info: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"


gen: ; (cd $(BUILD) && $(ISNOTSRCDIR) && $(MAKE) "$@") || exit ; $(MAKE) done "RULE=$@"
done: ;@ if grep "$(RULE)-done .*:" Makefile > /dev/null; then     echo $(MAKE) $(RULE)-done ; $(MAKE) $(RULE)-done ; else true ; fi
