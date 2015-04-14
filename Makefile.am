.PHONY: \
	logme

ACLOCAL_AMFLAGS = -I build

SUBDIRS = \
	lib \
	porg \
	grop \
	doc \
	scripts

pkgdata_DATA = \
	README

EXTRA_DIST = \
	ChangeLog.paco

logdir = \
	$(DESTDIR)@LOGDIR@

## Create the porg log directory on install, if it does not exist
install-exec-local:
	test -d $(logdir) || $(mkinstalldirs) $(logdir)

logme_files = \
	$(DESTDIR)$(pkgdatadir)/README

logme:
	@echo "'make logme' is no longer required to log the own porg installation."
	@echo "Try 'porg -lp porg-$(PACKAGE_VERSION) make install' instead."
	@echo "If it fails, try 'make logme-really' and report it, please."

## A rule to log the own porg's installation
logme-really:
	ls $(logme_files) | porg -lp porg-$(PACKAGE_VERSION) && \
	for dir in $(SUBDIRS); do \
	  ( cd $$dir && $(MAKE) logme ) 2>/dev/null; \
	done && \
	echo && porg -vvxfsty porg-$(PACKAGE_VERSION)

## Download SVN snapshot (Read Only)
svn-ro:
	svn checkout svn://svn.code.sf.net/p/porg/code/ porg-code

## Download SVN snapshot (Read / Write)
svn-rw:
	svn checkout --username=davidrr svn+ssh://davidrr@svn.code.sf.net/p/porg/code/ porg-code

README: doc/index.html
	html2text $< | $(SED) 's/&ast;/*/g' > README

