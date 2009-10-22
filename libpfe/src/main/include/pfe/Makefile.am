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
        os-ctype.h os-string.h os-setjmp.h os-fesetenv.h \
        os-delay.h p4-gettimeofday.h \
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
	struct-ext.h edit-ext.h \
	help-ext.h stackhelp-ext.h gforth-ext.h \
        assembler-ext.h \
	smart-go-ext.h stackhelp-ext.h
# note: on k12xx, we link dl-internal directly since we also link main-k12
# but the dl-internal knows only zchar-ext, the other modules are external.
