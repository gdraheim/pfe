ANSTESTS = 0.6
ANSTESTS_ZIP = anstests$(ANSTESTS).zip
ANSTESTS_URL = http://www.qlikz.org/forth/anstests/$(ANSTESTS_ZIP)

EXTRA_DIST = $(ANSTESTS_ZIP) anstests.txt

../pfe/pfe$(EXEEXT) :
	(cd ../pfe && $(MAKE) `basename $@`)

PFE_DIR = $(top_builddir)/pfe
PFE = $(PFE_DIR)/pfe$(EXEEXT)
PFE_LIB_PATH_STRING = --lib-path-string=$(PFE_DIR)/.libs
PFE_SCRIPTDIR_STRING = --inc-path-string=$(builddir)
PFE_CHECK_ARGS = --bye --quiet $(PFE_SCRIPTDIR_STRING) $(PFE_LIB_PATH_STRING)
UNZIP = unzip
WGET = wget

wget:
	test -f $(ANSTESTS_ZIP) || ( \
	test -s $(srcdir)/$(ANSTESTS_ZIP) && \
	     cp $(srcdir)/$(ANSTESTS_ZIP) $(ANSTESTS_ZIP) \
	) || $(WGET) $(ANSTESTS_URL)
	test -s $(ANSTESTS_ZIP)
anstests.txt:
	echo "downloaded from $(ANSTESTS_URL)" > $@

checks: \
	coreexttest coreplustest doubletest exceptiontest filetest \
	memorytest searchordertest stringtest toolstest core
        
coreexttest coreplustest doubletest exceptiontest filetest \
memorytest searchordertest stringtest toolstest:
	@ test -s $(ANSTESTS_ZIP) || $(MAKE) wget
	$(UNZIP) -o $(ANSTESTS_ZIP) tester.fr $@.fth
	$(PFE) $(PFE_CHECK_ARGS) -i tester.fr $@.fth

core:
	@ test -s $(ANSTESTS_ZIP) || $(MAKE) wget
	$(UNZIP) -o $(ANSTESTS_ZIP) tester.fr $@.fr
	cat $@.fr | sed -e "s/^T{ *ACCEPT-TEST/\\\\ &/" > $@.fth
	$(PFE) $(PFE_CHECK_ARGS) -i tester.fr $@.fth
