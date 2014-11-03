TARGETS	= $(OBJDIR)editor
OBJDIR	=
PREFIX	= /usr/local
DESTDIR	=
BINDIR	= $(PREFIX)/bin
SBINDIR	= $(PREFIX)/sbin
CC	= cc
CPPFLAGSF=
CPPFLAGS=
CFLAGSF	= -W `pkg-config --cflags libDesktop`
CFLAGS	= -Wall -g -O2 -pedantic
LDFLAGSF= `pkg-config --libs libDesktop` -lintl
LDFLAGS	= 
RM	= rm -f
LN	= ln -f
MKDIR	= mkdir -m 0755 -p
INSTALL	= install


all: $(TARGETS)

editor_OBJS = $(OBJDIR)callbacks.o $(OBJDIR)editor.o $(OBJDIR)filter.o $(OBJDIR)main.o
editor_CFLAGS = $(CPPFLAGSF) $(CPPFLAGS) $(CFLAGSF) $(CFLAGS)
editor_LDFLAGS = $(LDFLAGSF) $(LDFLAGS)

$(OBJDIR)editor: $(editor_OBJS)
	$(CC) -o $(OBJDIR)editor $(editor_OBJS) $(editor_LDFLAGS)

$(OBJDIR)callbacks.o: callbacks.c editor.h callbacks.h
	$(CC) $(editor_CFLAGS) -o $(OBJDIR)callbacks.o -c callbacks.c

$(OBJDIR)editor.o: editor.c callbacks.h editor.h ../config.h
	$(CC) $(editor_CFLAGS) -o $(OBJDIR)editor.o -c editor.c

$(OBJDIR)filter.o: filter.c filter.h
	$(CC) $(editor_CFLAGS) -o $(OBJDIR)filter.o -c filter.c

$(OBJDIR)main.o: main.c filter.h
	$(CC) $(editor_CFLAGS) -o $(OBJDIR)main.o -c main.c

clean:
	$(RM) -- $(editor_OBJS)

distclean: clean
	$(RM) -- $(TARGETS)

install: $(TARGETS)
	$(MKDIR) $(DESTDIR)$(BINDIR)
	$(INSTALL) -m 0755 $(OBJDIR)editor $(DESTDIR)$(BINDIR)/editor

uninstall:
	$(RM) -- $(DESTDIR)$(BINDIR)/editor

.PHONY: all clean distclean install uninstall
