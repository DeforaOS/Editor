PACKAGE	= Editor
VERSION	= 0.3.0
SUBDIRS	= data doc po src tools
RM	= rm -f
LN	= ln -f
TAR	= tar -czvf


all: subdirs

subdirs:
	@for i in $(SUBDIRS); do (cd "$$i" && $(MAKE)) || exit; done

clean:
	@for i in $(SUBDIRS); do (cd "$$i" && $(MAKE) clean) || exit; done

distclean:
	@for i in $(SUBDIRS); do (cd "$$i" && $(MAKE) distclean) || exit; done

dist:
	$(RM) -r -- $(PACKAGE)-$(VERSION)
	$(LN) -s -- . $(PACKAGE)-$(VERSION)
	@$(TAR) $(PACKAGE)-$(VERSION).tar.gz -- \
		$(PACKAGE)-$(VERSION)/data/Makefile \
		$(PACKAGE)-$(VERSION)/data/deforaos-editor.desktop \
		$(PACKAGE)-$(VERSION)/data/project.conf \
		$(PACKAGE)-$(VERSION)/doc/Makefile \
		$(PACKAGE)-$(VERSION)/doc/docbook.sh \
		$(PACKAGE)-$(VERSION)/doc/editor.xml \
		$(PACKAGE)-$(VERSION)/doc/filter.xml \
		$(PACKAGE)-$(VERSION)/doc/project.conf \
		$(PACKAGE)-$(VERSION)/po/Makefile \
		$(PACKAGE)-$(VERSION)/po/gettext.sh \
		$(PACKAGE)-$(VERSION)/po/POTFILES \
		$(PACKAGE)-$(VERSION)/po/de.po \
		$(PACKAGE)-$(VERSION)/po/es.po \
		$(PACKAGE)-$(VERSION)/po/fr.po \
		$(PACKAGE)-$(VERSION)/po/it.po \
		$(PACKAGE)-$(VERSION)/po/project.conf \
		$(PACKAGE)-$(VERSION)/src/callbacks.c \
		$(PACKAGE)-$(VERSION)/src/editor.c \
		$(PACKAGE)-$(VERSION)/src/filter.c \
		$(PACKAGE)-$(VERSION)/src/main.c \
		$(PACKAGE)-$(VERSION)/src/Makefile \
		$(PACKAGE)-$(VERSION)/src/callbacks.h \
		$(PACKAGE)-$(VERSION)/src/editor.h \
		$(PACKAGE)-$(VERSION)/src/filter.h \
		$(PACKAGE)-$(VERSION)/src/project.conf \
		$(PACKAGE)-$(VERSION)/tools/filter.c \
		$(PACKAGE)-$(VERSION)/tools/Makefile \
		$(PACKAGE)-$(VERSION)/tools/project.conf \
		$(PACKAGE)-$(VERSION)/Makefile \
		$(PACKAGE)-$(VERSION)/COPYING \
		$(PACKAGE)-$(VERSION)/config.h \
		$(PACKAGE)-$(VERSION)/config.sh \
		$(PACKAGE)-$(VERSION)/project.conf
	$(RM) -- $(PACKAGE)-$(VERSION)

install:
	@for i in $(SUBDIRS); do (cd "$$i" && $(MAKE) install) || exit; done

uninstall:
	@for i in $(SUBDIRS); do (cd "$$i" && $(MAKE) uninstall) || exit; done

.PHONY: all subdirs clean distclean dist install uninstall
