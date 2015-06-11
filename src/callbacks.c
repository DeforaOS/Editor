/* $Id$ */
/* Copyright (c) 2009-2015 Pierre Pronchery <khorben@defora.org> */
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



#include <stdlib.h>
#include <libintl.h>
#include <Desktop.h>
#include "editor.h"
#include "callbacks.h"
#include "../config.h"
#define _(string) gettext(string)


/* public */
/* functions */
gboolean on_closex(gpointer data)
{
	Editor * editor = data;

	editor_close(editor);
	return TRUE;
}


/* on_edit_copy */
void on_edit_copy(gpointer data)
{
	Editor * editor = data;

	editor_copy(editor);
}


/* on_edit_cut */
void on_edit_cut(gpointer data)
{
	Editor * editor = data;

	editor_cut(editor);
}


/* on_edit_find */
void on_edit_find(gpointer data)
{
	Editor * editor = data;

	on_find(editor);
}


/* on_edit_paste */
void on_edit_paste(gpointer data)
{
	Editor * editor = data;

	editor_paste(editor);
}


/* on_edit_preferences */
void on_edit_preferences(gpointer data)
{
	Editor * editor = data;

	editor_show_preferences(editor, TRUE);
}


/* on_edit_select_all */
void on_edit_select_all(gpointer data)
{
	Editor * editor = data;

	editor_select_all(editor);
}


/* on_edit_unselect_all */
void on_edit_unselect_all(gpointer data)
{
	Editor * editor = data;

	editor_unselect_all(editor);
}


/* on_file_close */
void on_file_close(gpointer data)
{
	Editor * editor = data;

	editor_close(editor);
}


/* on_file_new */
void on_file_new(gpointer data)
{
	Editor * editor = data;

	editor_open(editor, NULL);
}


/* on_file_open */
void on_file_open(gpointer data)
{
	Editor * editor = data;

	editor_open_dialog(editor);
}


/* on_file_print */
void on_file_print(gpointer data)
{
	Editor * editor = data;

	editor_print_dialog(editor);
}


/* on_file_properties */
void on_file_properties(gpointer data)
{
	Editor * editor = data;

	editor_show_properties(editor, TRUE);
}


/* on_file_save */
void on_file_save(gpointer data)
{
	Editor * editor = data;

	editor_save(editor);
}


/* on_file_save_as */
void on_file_save_as(gpointer data)
{
	Editor * editor = data;

	editor_save_as_dialog(editor);
}


/* on_help_about */
void on_help_about(gpointer data)
{
	Editor * editor = data;

	editor_about(editor);
}


/* on_help_contents */
void on_help_contents(gpointer data)
{
	desktop_help_contents(PACKAGE, "editor");
}


/* on_insert_file */
void on_insert_file(gpointer data)
{
	Editor * editor = data;

	editor_insert_file_dialog(editor);
}


/* toolbar */
/* on_close */
void on_close(gpointer data)
{
	Editor * editor = data;

	editor_close(editor);
}


/* on_copy */
void on_copy(gpointer data)
{
	Editor * editor = data;

	editor_copy(editor);
}


/* on_cut */
void on_cut(gpointer data)
{
	Editor * editor = data;

	editor_cut(editor);
}


/* on_find */
void on_find(gpointer data)
{
	Editor * editor = data;

	editor_find(editor, NULL);
}


/* on_new */
void on_new(gpointer data)
{
	Editor * editor = data;

	editor_open(editor, NULL);
}


/* on_open */
void on_open(gpointer data)
{
	Editor * editor = data;

	editor_open_dialog(editor);
}


/* on_paste */
void on_paste(gpointer data)
{
	Editor * editor = data;

	editor_paste(editor);
}


/* on_print */
void on_print(gpointer data)
{
	Editor * editor = data;

	editor_print_dialog(editor);
}


/* on_save */
void on_save(gpointer data)
{
	Editor * editor = data;

	editor_save(editor);
}


/* on_save_as */
void on_save_as(gpointer data)
{
	Editor * editor = data;

	editor_save_as_dialog(editor);
}


/* on_preferences */
void on_preferences(gpointer data)
{
	Editor * editor = data;

	on_edit_preferences(editor);
}


/* on_properties */
void on_properties(gpointer data)
{
	Editor * editor = data;

	on_file_properties(editor);
}
