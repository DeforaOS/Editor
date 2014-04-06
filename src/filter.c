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
