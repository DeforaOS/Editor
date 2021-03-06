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



#ifndef EDITOR_CALLBACKS_H
# define EDITOR_CALLBACKS_H

# include <gtk/gtk.h>


/* functions */
gboolean on_closex(gpointer data);
void on_edit_copy(gpointer data);
void on_edit_cut(gpointer data);
void on_edit_find(gpointer data);
void on_edit_paste(gpointer data);
void on_edit_preferences(gpointer data);
void on_edit_select_all(gpointer data);
void on_edit_unselect_all(gpointer data);
void on_file_close(gpointer data);
void on_file_new(gpointer data);
void on_file_open(gpointer data);
void on_file_print(gpointer data);
void on_file_properties(gpointer data);
void on_file_save(gpointer data);
void on_file_save_as(gpointer data);
void on_help_about(gpointer data);
void on_help_contents(gpointer data);
void on_insert_file(gpointer data);

/* toolbar */
void on_close(gpointer data);
void on_copy(gpointer data);
void on_cut(gpointer data);
void on_find(gpointer data);
void on_new(gpointer data);
void on_open(gpointer data);
void on_paste(gpointer data);
void on_preferences(gpointer data);
void on_print(gpointer data);
void on_properties(gpointer data);
void on_save(gpointer data);
void on_save_as(gpointer data);

#endif /* !EDITOR_CALLBACKS_H */
