TARGETS	= filter
PREFIX	= /usr/local
DESTDIR	=
BINDIR	= $(PREFIX)/bin
SBINDIR	= $(PREFIX)/sbin
CC	= cc
CPPFLAGSF=
CPPFLAGS=
CFLAGSF	= -W
CFLAGS	= -Wall -g -O2 -pedantic
LDFLAGSF= -lintl
LDFLAGS	= 
RM	= rm -f
LN	= ln -f
MKDIR	= mkdir -m 0755 -p
INSTALL	= install


all: $(TARGETS)

filter_OBJS = filter.o
filter_CFLAGS = $(CPPFLAGSF) $(CPPFLAGS) $(CFLAGSF) $(CFLAGS)
filter_LDFLAGS = $(LDFLAGSF) $(LDFLAGS) `pkg-config --libs libSystem` ../src/filter.o

filter: $(filter_OBJS)
	$(CC) -o filter $(filter_OBJS) $(filter_LDFLAGS)

filter.o: filter.c
	$(CC) $(filter_CFLAGS) -c filter.c

clean:
	$(RM) -- $(filter_OBJS)

distclean: clean
	$(RM) -- $(TARGETS)

install: $(TARGETS)

uninstall:

.PHONY: all clean distclean install uninstall
