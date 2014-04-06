/* $Id$ */
/* Copyright (c) 2014 Pierre Pronchery <khorben@defora.org> */
/* This file is part of DeforaOS Desktop Editor */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>. */



#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <libintl.h>
#include "../src/filter.h"
#include "../config.h"
#define _(string) gettext(string)

/* constants */
#ifndef PROGNAME
# define PROGNAME	"filter"
#endif
#ifndef PREFIX
# define PREFIX		"/usr/local"
#endif
#ifndef DATADIR
# define DATADIR	PREFIX "/share"
#endif
#ifndef LOCALEDIR
# define LOCALEDIR	DATADIR "/locale"
#endif


/* filter */
/* private */
/* prototypes */
static int _filter(int argc, char const * argv[]);

static int _error(char const * message, int ret);
static int _usage(void);


/* functions */
/* filter */
static int _filter_exec(char const * template, int argc, char const ** argv);

static int _filter(int argc, char const * argv[])
{
	int ret = 0;
	char template[] = "/tmp/" PROGNAME ".XXXXXX";
	int fd;

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s()\n", __func__);
#endif
	/* obtain a temporary file */
	if((fd = mkstemp(template)) < 0)
		return -_error("mkstemp", 1);
#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s() \"%s\"\n", __func__, template);
#endif
	/* write to and from the temporary file */
	if((ret = filter_read(fd, template)) == 0
			&& (ret = _filter_exec(template, argc, argv)) == 0)
		ret = filter_write(template);
	/* remove the temporary file */
	if(unlink(template) != 0)
		/* we can otherwise ignore this error */
		_error(template, 1);
	return ret;
}

static int _filter_exec(char const * template, int argc, char const ** argv)
{
	int ret = 0;
	char ** args;
	int i;

	if((args = malloc(sizeof(*args) * (argc + 1))) == NULL)
		return -_error("malloc", 1);
	for(i = 0; i < argc; i++)
		args[i] = strdup(argv[i]);
	args[i] = strdup(template);
	/* check for errors */
	for(i = 0; ret == 0 && i <= argc; i++)
		if(args[i] == NULL)
			ret = -_error("strdup", 1);
	/* FIXME fork() first */
	if(ret == 0 && execvp(args[0], args) != 0)
		ret = -_error(args[0], 1);
	for(i = 0; i <= argc; i++)
		free(args[i]);
	free(args);
	return ret;
}


/* error */
static int _error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* usage */
static int _usage(void)
{
	fprintf(stderr, _("Usage: %s [-F][filename]\n"
"  -F	Behave like a filter\n"), PROGNAME);
	return 1;
}


/* main */
int main(int argc, char * argv[])
{
	int o;

	if(setlocale(LC_ALL, "") == NULL)
		_error("setlocale", 1);
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	while((o = getopt(argc, argv, "")) != -1)
		switch(o)
		{
			default:
				return _usage();
		}
	if(argc - optind == 0)
		return _usage();
	return (_filter(argc - optind, (const char **)&argv[optind]) == 0)
		? 0 : 2;
}
