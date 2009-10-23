AUTOMAKE_OPTIONS = 1.5 foreign dist-bzip2
AUTOTOOL_VERSION = autoconf-2.52 automake-1.5 libtool-1.4.2
ACLOCAL_AMFLAGS = -I ../config/m4

LIBS   = @PFE_LIBS@ @LIBS@
CFLAGS = @PFE_CFLAGS@ @CFLAGS@ -I..
pfelibdir = @PFE_LIBDIR@
pfesuffix = @PFE_SUFFIX@
pfepluginsdir = ${pfelibdir}${pfesuffix}
pfeplugins_LTLIBRARIES = socket.la smart-go.la gettimeofday.la

socket_la_SOURCES = src/main/c/socket-dll.c src/main/c/socket-ext.c
socket_la_LDFLAGS = -export-dynamic -module -avoid-version
src/main/c/socket-dll.c : @pfeincludedir@/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed -e s/module/socket/ "$?" >> $@

smart_go_la_SOURCES = src/main/c/smart-go-dll.c src/main/c/smart-go-ext.c
smart_go_la_LDFLAGS = -export-dynamic -module -avoid-version
src/main/c/smart-go-dll.c : @pfeincludedir@/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/smart_go/ "$?" >> $@

struct_la_SOURCES = src/main/c/struct-dll.c src/main/c/struct-ext.c
struct_la_LDFLAGS = -export-dynamic -module -avoid-version
src/main/c/struct-dll.c : @pfeincludedir@/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/struct/ "$?" >> $@

structs_la_SOURCES = src/main/c/structs-dll.c src/main/c/structs-ext.c 
structs_la_LDFLAGS = -export-dynamic -module -avoid-version
src/main/c/structs-dll.c : @pfeincludedir@/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/structs/ "$?" >> $@

gettimeofday_la_SOURCES = src/main/c/gettimeofday-dll.c src/main/c/gettimeofday-ext.c 
gettimeofday_la_LDFLAGS = -export-dynamic -module -avoid-version
src/main/c/gettimeofday-dll.c : @pfeincludedir@/pfe/module-dll.inc
	echo "/* generated from pfe/module-dll.inc */" > $@
	sed s/module/gettimeofday/ "$?" >> $@

EXTRA_DIST = src/main/c/lib-sdl.c
