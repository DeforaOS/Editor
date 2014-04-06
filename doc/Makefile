TARGETS	= editor.1 editor.html filter.1 filter.html
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

filter.1: filter.xml
	./docbook.sh -P "$(PREFIX)" -- "filter.1"

filter.html: filter.xml
	./docbook.sh -P "$(PREFIX)" -- "filter.html"

clean:
	$(RM) -- $(editor.1_OBJS) $(editor.html_OBJS) $(filter.1_OBJS) $(filter.html_OBJS)
	./docbook.sh -c -P "$(PREFIX)" -- "filter.html"

distclean: clean
	$(RM) -- $(TARGETS)

install: $(TARGETS)
	./docbook.sh -P "$(DESTDIR)$(PREFIX)" -i -- "editor.1"
	./docbook.sh -P "$(DESTDIR)$(PREFIX)" -i -- "editor.html"
	./docbook.sh -P "$(DESTDIR)$(PREFIX)" -i -- "filter.1"
	./docbook.sh -P "$(DESTDIR)$(PREFIX)" -i -- "filter.html"

uninstall:
	./docbook.sh -P "$(DESTDIR)$(PREFIX)" -u -- "editor.1"
	./docbook.sh -P "$(DESTDIR)$(PREFIX)" -u -- "editor.html"
	./docbook.sh -P "$(DESTDIR)$(PREFIX)" -u -- "filter.1"
	./docbook.sh -P "$(DESTDIR)$(PREFIX)" -u -- "filter.html"

.PHONY: all clean distclean install uninstall
