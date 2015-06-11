/* $Id$ */
/* Copyright (c) 2006-2015 Pierre Pronchery <khorben@defora.org> */
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



#ifndef EDITOR_EDITOR_H
# define EDITOR_EDITOR_H

# include <System.h>
# include <gtk/gtk.h>


/* Editor */
/* types */
typedef struct _EditorPrefs
{
	int filter;
} EditorPrefs;

typedef struct _Editor Editor;


/* functions */
Editor * editor_new(EditorPrefs * prefs);
void editor_delete(Editor * editor);

/* accessors */
char const * editor_get_font(Editor * editor);
GtkWrapMode editor_get_wrap_mode(Editor * editor);
void editor_set_font(Editor * editor, char const * font);
void editor_set_wrap_mode(Editor * editor, GtkWrapMode wrap);

/* useful */
void editor_about(Editor * editor);

int editor_config_load(Editor * editor);
int editor_config_save(Editor * editor);

int editor_confirm(Editor * editor, char const * message, ...);
int editor_error(Editor * editor, char const * message, int ret);

int editor_close(Editor * editor);
int editor_open(Editor * editor, char const * filename);
int editor_open_dialog(Editor * editor);

void editor_copy(Editor * editor);
void editor_cut(Editor * editor);
void editor_paste(Editor * editor);

int editor_insert_file(Editor * editor, char const * filename);
int editor_insert_file_dialog(Editor * editor);

void editor_print_dialog(Editor * editor);

gboolean editor_save(Editor * editor);
gboolean editor_save_as(Editor * editor, char const * filename);
gboolean editor_save_as_dialog(Editor * editor);

void editor_select_all(Editor * editor);
void editor_unselect_all(Editor * editor);

void editor_find(Editor * editor, char const * text);

void editor_show_preferences(Editor * editor, gboolean show);
void editor_show_properties(Editor * editor, gboolean show);

#endif /* !EDITOR_EDITOR_H */
