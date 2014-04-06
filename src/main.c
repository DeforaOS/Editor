/* $Id$ */
/* Copyright (c) 2006-2014 Pierre Pronchery <khorben@defora.org> */
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
#include "filter.h"
#include "editor.h"
#include "../config.h"
#define _(string) gettext(string)

/* constants */
#ifndef PROGNAME
# define PROGNAME	"editor"
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


/* Editor */
/* private */
/* prototypes */
static int _editor(EditorPrefs * prefs, char const * filename);
static int _editor_filter(EditorPrefs * prefs);

static int _error(char const * message, int ret);
static int _usage(void);


/* functions */
/* editor */
static int _editor(EditorPrefs * prefs, char const * filename)
{
	Editor * editor;

	if((editor = editor_new(prefs)) == NULL)
		return -1;
	if(filename != NULL)
		editor_open(editor, filename);
	gtk_main();
	editor_delete(editor);
	return 0;
}


/* editor_filter */
static int _editor_filter(EditorPrefs * prefs)
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
	if((ret = filter_read(fd, template)) != 0
			|| (ret = _editor(prefs, template)) != 0
			|| (ret = filter_write(template)) != 0)
		error_print(PROGNAME);
	/* remove the temporary file */
	if(unlink(template) != 0)
		/* we can otherwise ignore this error */
		_error(template, 1);
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
	EditorPrefs prefs;
	int o;

	memset(&prefs, 0, sizeof(prefs));
	if(setlocale(LC_ALL, "") == NULL)
		_error("setlocale", 1);
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	gtk_init(&argc, &argv);
	while((o = getopt(argc, argv, "F")) != -1)
		switch(o)
		{
			case 'F':
				prefs.filter = 1;
				break;
			default:
				return _usage();
		}
	if(prefs.filter != 0 && optind == argc)
		return (_editor_filter(&prefs) == 0) ? 0 : 2;
	/* ignore filter mode if a filename was supplied */
	prefs.filter = 0;
	if(optind != argc && optind + 1 != argc)
		return _usage();
	if(argc - optind == 1)
		return (_editor(&prefs, argv[optind]) == 0) ? 0 : 2;
	return (_editor(&prefs, NULL) == 0) ? 0 : 2;
}
