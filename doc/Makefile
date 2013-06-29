TARGETS	= editor.1 editor.html
PREFIX	= /usr/local
DESTDIR	=
RM	= rm -f
LN	= ln -f
MKDIR	= mkdir -m 0755 -p
INSTALL	= install


all: $(TARGETS)

editor.1: editor.xml
	./docbook.sh -P "$(PREFIX)" -- "editor.1"

editor.html: editor.xml
	./docbook.sh -P "$(PREFIX)" -- "editor.html"

clean:
	$(RM) -- $(editor.1_OBJS) $(editor.html_OBJS)
	./docbook.sh -c -P "$(PREFIX)" -- "editor.html"

distclean: clean
	$(RM) -- $(TARGETS)

install: $(TARGETS)
	./docbook.sh -P "$(DESTDIR)$(PREFIX)" -i -- "editor.1"
	./docbook.sh -P "$(DESTDIR)$(PREFIX)" -i -- "editor.html"

uninstall:
	./docbook.sh -P "$(DESTDIR)$(PREFIX)" -u -- "editor.1"
	./docbook.sh -P "$(DESTDIR)$(PREFIX)" -u -- "editor.html"

.PHONY: all clean distclean install uninstall
