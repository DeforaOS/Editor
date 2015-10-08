/* $Id$ */
/* Copyright (c) 2014 Pierre Pronchery <khorben@defora.org> */
/* This file is part of DeforaOS Desktop Editor */
/* All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */



#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <errno.h>
#include <libintl.h>
#include <System.h>
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
static int _filter_exec_child(char const * template, int argc,
		char const ** argv);

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
	if((ret = filter_read(fd, template)) != 0
			|| (ret = _filter_exec(template, argc, argv)) != 0
			|| (ret = filter_write(template)) != 0)
		error_print(PROGNAME);
	/* remove the temporary file */
	if(unlink(template) != 0)
		/* we can otherwise ignore this error */
		_error(template, 1);
	return ret;
}

static int _filter_exec(char const * template, int argc, char const ** argv)
{
	int ret = 0;
	pid_t pid;
	int status;
	pid_t p;

	if((pid = fork()) == -1)
		return -error_set_code(1, "%s: %s", "fork", strerror(errno));
	if(pid == 0)
	{
		if(_filter_exec_child(template, argc, argv) != 0)
			error_print(PROGNAME);
		_exit(2);
	}
	fclose(stdin);
	for(; (p = waitpid(pid, &status, 0)) != -1 || errno != ECHILD;)
		if(p == pid && WIFEXITED(status))
		{
			if(WEXITSTATUS(status) != 0)
				ret = -error_set_code(1, "%s: %s%u", argv[0],
						_("Exited with error code "),
						WEXITSTATUS(status));
			break;
		}
		else if(p == pid && WIFSIGNALED(status))
			break;
	return ret;
}

static int _filter_exec_child(char const * template, int argc,
		char const ** argv)
{
	int ret = 0;
	char ** args;
	int i;

	/* prepare the arguments */
	if((args = malloc(sizeof(*args) * (argc + 1))) == NULL)
		return -_error("malloc", 1);
	for(i = 0; i < argc; i++)
		args[i] = strdup(argv[i]);
	args[i] = strdup(template);
	/* check for errors */
	for(i = 0; ret == 0 && i <= argc; i++)
		if(args[i] == NULL)
			ret = -_error("strdup", 1);
	if(ret == 0 && execvp(args[0], args) != 0)
		ret = -_error(args[0], 1);
	/* free the arguments (an error occurred) */
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
	fprintf(stderr, _("Usage: %s command [argument...]\n"), PROGNAME);
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
