TARGETS	= $(OBJDIR)filter
OBJDIR	=
PREFIX	= /usr/local
DESTDIR	=
BINDIR	= $(PREFIX)/bin
SBINDIR	= $(PREFIX)/sbin
CC	= cc
CPPFLAGSF=
CPPFLAGS=
CFLAGSF	= -W `pkg-config --cflags libSystem`
CFLAGS	= -Wall -g -O2 -pedantic
LDFLAGSF= `pkg-config --libs libSystem` -lintl
LDFLAGS	= 
RM	= rm -f
LN	= ln -f
MKDIR	= mkdir -m 0755 -p
INSTALL	= install


all: $(TARGETS)

filter_OBJS = $(OBJDIR)filter.o
filter_CFLAGS = $(CPPFLAGSF) $(CPPFLAGS) $(CFLAGSF) $(CFLAGS)
filter_LDFLAGS = $(LDFLAGSF) $(LDFLAGS) ../src/filter.o

$(OBJDIR)filter: $(filter_OBJS)
	$(CC) -o $(OBJDIR)filter $(filter_OBJS) $(filter_LDFLAGS)

$(OBJDIR)filter.o: filter.c
	$(CC) $(filter_CFLAGS) -o $(OBJDIR)filter.o -c filter.c

clean:
	$(RM) -- $(filter_OBJS)

distclean: clean
	$(RM) -- $(TARGETS)

install: $(TARGETS)
	$(MKDIR) $(DESTDIR)$(BINDIR)
	$(INSTALL) -m 0755 $(OBJDIR)filter $(DESTDIR)$(BINDIR)/filter

uninstall:
	$(RM) -- $(DESTDIR)$(BINDIR)/filter

.PHONY: all clean distclean install uninstall
