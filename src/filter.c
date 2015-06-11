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



#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <System.h>
#include "filter.h"


/* public */
/* functions */
/* filter_read */
int filter_read(int fd, char const * template)
{
	int ret;
	FILE * fp;
	char buf[BUFSIZ];
	size_t s;

	if((fp = fdopen(fd, "w")) == NULL)
	{
		unlink(template);
		return -error_set_code(1, "%s: %s", "stdin", strerror(errno));
	}
	/* read from the standard input */
	for(ret = 0; ret == 0 && (s = fread(buf, sizeof(*buf), sizeof(buf)
					/ sizeof(*buf), stdin)) > 0;)
		/* write to the temporary file */
		if(fwrite(buf, sizeof(*buf), s, fp) != s)
			ret = -error_set_code(1, "%s: %s", template,
					strerror(errno));
	if(ret == 0 && !feof(stdin))
		ret = -error_set_code(1, "%s: %s", "stdin", strerror(errno));
	if(fclose(fp) != 0 && ret == 0)
		ret = -error_set_code(1, "%s: %s", template, strerror(errno));
	return ret;
}


/* filter_write */
int filter_write(char const * template)
{
	int ret;
	FILE * fp;
	char buf[BUFSIZ];
	size_t s;

	if((fp = fopen(template, "r")) == NULL)
		return -error_set_code(1, "%s: %s", template, strerror(errno));
	/* read from the temporary file */
	for(ret = 0; (s = fread(buf, sizeof(*buf), sizeof(buf) / sizeof(*buf),
					fp)) > 0;)
		/* write to the standard output */
		if(fwrite(buf, sizeof(*buf), s, stdout) != s)
			ret = -error_set_code(1, "%s: %s", "stdout",
					strerror(errno));
	if(ret == 0 && !feof(fp))
		ret = -error_set_code(1, "%s: %s", template, strerror(errno));
	if(fclose(fp) != 0 && ret == 0)
		ret = -error_set_code(1, "%s: %s", template, strerror(errno));
	return ret;
}
