/* $Id$ */
static char const _copyright[] =
"Copyright © 2006-2016 Pierre Pronchery <khorben@defora.org>";
/* This file is part of DeforaOS Desktop Editor */
static char const _license[] =
"All rights reserved.\n"
"\n" 
"Redistribution and use in source and binary forms, with or without\n"
"modification, are permitted provided that the following conditions are\n"
"met:\n"
"\n"
"1. Redistributions of source code must retain the above copyright\n"
"   notice, this list of conditions and the following disclaimer.\n"
"\n"
"2. Redistributions in binary form must reproduce the above copyright\n"
"   notice, this list of conditions and the following disclaimer in the\n"
"   documentation and/or other materials provided with the distribution.\n"
"\n"
"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS\n"
"IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED\n"
"TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A\n"
"PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT\n"
"HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,\n"
"SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED\n"
"TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR\n"
"PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF\n"
"LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING\n"
"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\n"
"SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.";
/* TODO:
 * - add a "Back" button to the "Find" dialog
 * - add a "Replace" dialog
 * - consider using GtkSourceView also/instead */



#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <math.h>
#include <libintl.h>
#include <gdk/gdkkeysyms.h>
#include <Desktop.h>
#include "callbacks.h"
#include "editor.h"
#include "../config.h"
#define _(string) gettext(string)
#define N_(string) (string)


/* Editor */
/* private */
/* constants */
#ifndef PROGNAME
# define PROGNAME		"editor"
#endif
#define EDITOR_CONFIG_FILE	".editor"
#define EDITOR_DEFAULT_FONT	"Monospace 9"


/* types */
struct _Editor
{
	EditorPrefs prefs;
	gchar * filename;
	size_t search;

	Config * config;

	/* widgets */
	GtkWidget * window;
	PangoFontDescription * bold;
	GtkWidget * view;
	GtkWidget * statusbar;
	guint statusbar_id;
#if GTK_CHECK_VERSION(2, 18, 0)
	/* infobar */
	GtkWidget * infobar;
	GtkWidget * infobar_label;
#endif
	/* preferences */
	GtkWidget * pr_window;
	GtkWidget * pr_font;
	GtkWidget * pr_wrap;
	/* find */
	GtkWidget * fi_dialog;
	GtkListStore * fi_store;
	GtkWidget * fi_text;
	GtkWidget * fi_entry;
	GtkWidget * fi_case;
	GtkWidget * fi_wrap;
	/* about */
	GtkWidget * ab_window;

	/* printing */
	PangoFontDescription * font;
	double font_size;
	double line_space;
	GtkTextIter iter;
	guint line_count;
};


/* variables */
static char const * _authors[] =
{
	"Pierre Pronchery <khorben@defora.org>",
	NULL
};

#ifdef EMBEDDED
static const DesktopAccel _editor_accel[] =
{
	{ G_CALLBACK(on_close), GDK_CONTROL_MASK, GDK_KEY_W },
	{ G_CALLBACK(on_new), GDK_CONTROL_MASK, GDK_KEY_N },
	{ G_CALLBACK(on_open), GDK_CONTROL_MASK, GDK_KEY_O },
	{ G_CALLBACK(on_save), GDK_CONTROL_MASK, GDK_KEY_S },
	{ G_CALLBACK(on_save_as), GDK_CONTROL_MASK | GDK_SHIFT_MASK,
		GDK_KEY_S },
	{ NULL, 0, 0 }
};
#endif

#ifndef EMBEDDED
static const DesktopMenu _editor_menu_file[] =
{
	{ N_("_New"), G_CALLBACK(on_file_new), GTK_STOCK_NEW, GDK_CONTROL_MASK,
		GDK_KEY_N },
	{ N_("_Open"), G_CALLBACK(on_file_open), GTK_STOCK_OPEN,
		GDK_CONTROL_MASK, GDK_KEY_O },
	{ "", NULL, NULL, 0, 0 },
	{ N_("_Save"), G_CALLBACK(on_file_save), GTK_STOCK_SAVE,
		GDK_CONTROL_MASK, GDK_KEY_S },
	{ N_("Save _As..."), G_CALLBACK(on_file_save_as), GTK_STOCK_SAVE_AS,
		GDK_CONTROL_MASK | GDK_SHIFT_MASK, GDK_KEY_S },
	{ "", NULL, NULL, 0, 0 },
	{ N_("_Print"), G_CALLBACK(on_file_print),
		GTK_STOCK_PRINT, GDK_CONTROL_MASK, GDK_KEY_P },
	{ "", NULL, NULL, 0, 0 },
	{ N_("Pr_operties"), G_CALLBACK(on_file_properties),
		GTK_STOCK_PROPERTIES, GDK_MOD1_MASK, GDK_KEY_Return },
	{ "", NULL, NULL, 0, 0 },
	{ N_("_Close"), G_CALLBACK(on_file_close), GTK_STOCK_CLOSE, 0, 0 },
	{ NULL, NULL, NULL, 0, 0 }
};

static const DesktopMenu _editor_menu_file_filter[] =
{
	{ N_("_Save"), G_CALLBACK(on_file_save), GTK_STOCK_SAVE,
		GDK_CONTROL_MASK, GDK_KEY_S },
	{ "", NULL, NULL, 0, 0 },
	{ N_("_Print"), G_CALLBACK(on_file_print),
		GTK_STOCK_PRINT, GDK_CONTROL_MASK, GDK_KEY_P },
	{ "", NULL, NULL, 0, 0 },
	{ N_("Pr_operties"), G_CALLBACK(on_file_properties),
		GTK_STOCK_PROPERTIES, GDK_MOD1_MASK, GDK_KEY_Return },
	{ "", NULL, NULL, 0, 0 },
	{ N_("_Close"), G_CALLBACK(on_file_close), GTK_STOCK_CLOSE, 0, 0 },
	{ NULL, NULL, NULL, 0, 0 }
};

static const DesktopMenu _editor_menu_edit[] =
{
	{ N_("Cu_t"), G_CALLBACK(on_edit_cut), GTK_STOCK_CUT, GDK_CONTROL_MASK,
		GDK_KEY_X },
	{ N_("_Copy"), G_CALLBACK(on_edit_copy), GTK_STOCK_COPY,
		GDK_CONTROL_MASK, GDK_KEY_C },
	{ N_("_Paste"), G_CALLBACK(on_edit_paste), GTK_STOCK_PASTE,
		GDK_CONTROL_MASK, GDK_KEY_V },
	{ "", NULL, NULL, 0, 0 },
	{ N_("Select _All"), G_CALLBACK(on_edit_select_all),
#if GTK_CHECK_VERSION(2, 10, 0)
		GTK_STOCK_SELECT_ALL,
#else
		"edit-select-all",
#endif
		GDK_CONTROL_MASK, GDK_KEY_A },
	{ N_("_Unselect all"), G_CALLBACK(on_edit_unselect_all), NULL, 0, 0 },
	{ "", NULL, NULL, 0, 0 },
	{ N_("_Find"), G_CALLBACK(on_edit_find), GTK_STOCK_FIND,
		GDK_CONTROL_MASK, GDK_KEY_F },
	{ "", NULL, NULL, 0, 0 },
	{ N_("_Preferences"), G_CALLBACK(on_edit_preferences),
		GTK_STOCK_PREFERENCES, 0, 0 },
	{ NULL, NULL, NULL, 0, 0 }
};

static const DesktopMenu _editor_menu_insert[] =
{
	{ N_("_File..."), G_CALLBACK(on_insert_file), "insert-text", 0, 0 },
	{ NULL, NULL, NULL, 0, 0 }
};

static const DesktopMenu _editor_menu_help[] =
{
	{ N_("_Contents"), G_CALLBACK(on_help_contents), "help-contents",
		0, GDK_KEY_F1 },
	{ N_("_About"), G_CALLBACK(on_help_about),
#if GTK_CHECK_VERSION(2, 6, 0)
		GTK_STOCK_ABOUT, 0, 0 },
#else
		NULL, 0, 0 },
#endif
	{ NULL, NULL, NULL, 0, 0 }
};

static const DesktopMenubar _editor_menubar[] =
{
	{ N_("_File"), _editor_menu_file },
	{ N_("_Edit"), _editor_menu_edit },
	{ N_("_Insert"), _editor_menu_insert },
	{ N_("_Help"), _editor_menu_help },
	{ NULL, NULL }
};

static const DesktopMenubar _editor_menubar_filter[] =
{
	{ N_("_File"), _editor_menu_file_filter },
	{ N_("_Edit"), _editor_menu_edit },
	{ N_("_Insert"), _editor_menu_insert },
	{ N_("_Help"), _editor_menu_help },
	{ NULL, NULL }
};
#endif

static DesktopToolbar _editor_toolbar[] =
{
	{ N_("New"), G_CALLBACK(on_new), GTK_STOCK_NEW, 0, 0, NULL },
	{ N_("Open"), G_CALLBACK(on_open), GTK_STOCK_OPEN, 0, 0, NULL },
	{ "", NULL, NULL, 0, 0, NULL },
	{ N_("Save"), G_CALLBACK(on_save), GTK_STOCK_SAVE, 0, 0, NULL },
	{ N_("Save as"), G_CALLBACK(on_save_as), GTK_STOCK_SAVE_AS, 0, 0,
		NULL },
	{ "", NULL, NULL, 0, 0, NULL },
	{ N_("Cut"), G_CALLBACK(on_cut), GTK_STOCK_CUT, 0, 0, NULL },
	{ N_("Copy"), G_CALLBACK(on_copy), GTK_STOCK_COPY, 0, 0, NULL },
	{ N_("Paste"), G_CALLBACK(on_paste), GTK_STOCK_PASTE, 0, 0, NULL },
#ifdef EMBEDDED
	{ "", NULL, NULL, 0, 0, NULL },
	{ N_("Print"), G_CALLBACK(on_print), GTK_STOCK_PREFERENCES,
		GDK_CONTROL_MASK, GDK_KEY_P, NULL },
	{ "", NULL, NULL, 0, 0, NULL },
	{ N_("Find"), G_CALLBACK(on_find), GTK_STOCK_FIND, GDK_CONTROL_MASK,
		GDK_KEY_F, NULL },
	{ "", NULL, NULL, 0, 0, NULL },
	{ N_("Preferences"), G_CALLBACK(on_preferences), GTK_STOCK_PREFERENCES,
		0, 0, NULL },
	{ N_("Properties"), G_CALLBACK(on_properties), GTK_STOCK_PROPERTIES,
		GDK_MOD1_MASK, GDK_KEY_Return, NULL },
	{ "", NULL, NULL, 0, 0, NULL },
	{ N_("Help"), G_CALLBACK(on_help_contents), "help-contents",
		0, GDK_KEY_F1, NULL },
#endif
	{ NULL, NULL, NULL, 0, 0, NULL }
};

static DesktopToolbar _editor_toolbar_filter[] =
{
	{ N_("Save"), G_CALLBACK(on_save), GTK_STOCK_SAVE, 0, 0, NULL },
	{ "", NULL, NULL, 0, 0, NULL },
	{ N_("Cut"), G_CALLBACK(on_cut), GTK_STOCK_CUT, 0, 0, NULL },
	{ N_("Copy"), G_CALLBACK(on_copy), GTK_STOCK_COPY, 0, 0, NULL },
	{ N_("Paste"), G_CALLBACK(on_paste), GTK_STOCK_PASTE, 0, 0, NULL },
#ifdef EMBEDDED
	{ "", NULL, NULL, 0, 0, NULL },
	{ N_("Find"), G_CALLBACK(on_find), GTK_STOCK_FIND, GDK_CONTROL_MASK,
		GDK_KEY_F, NULL },
	{ "", NULL, NULL, 0, 0, NULL },
	{ N_("Preferences"), G_CALLBACK(on_preferences), GTK_STOCK_PREFERENCES,
		0, 0, NULL },
	{ N_("Properties"), G_CALLBACK(on_properties), GTK_STOCK_PROPERTIES,
		GDK_MOD1_MASK, GDK_KEY_Return, NULL },
	{ "", NULL, NULL, 0, 0, NULL },
	{ N_("Help"), G_CALLBACK(on_help_contents), "help-contents",
		0, GDK_KEY_F1, NULL },
#endif
	{ NULL, NULL, NULL, 0, 0, NULL }
};

static struct
{
	char const * name;
	GtkWrapMode wrap;
} _editor_wrap[] =
{
	{ N_("none"),			GTK_WRAP_NONE		},
	{ N_("characters"),		GTK_WRAP_CHAR		},
	{ N_("words"),			GTK_WRAP_WORD		},
	{ N_("words then characters"),	GTK_WRAP_WORD_CHAR	}
};


/* prototypes */
static int _editor_set_filename(Editor * editor, char const * filename);
static void _editor_set_status(Editor * editor, char const * status);

static char * _editor_config_filename(void);
static gboolean _editor_find(Editor * editor, char const * text,
		gboolean sensitive, gboolean wrap);
static void _editor_refresh_title(Editor * editor);

/* callbacks */
#if GTK_CHECK_VERSION(2, 16, 0)
static void _editor_on_find_clear(gpointer data);
#endif
static void _editor_on_find_clicked(gpointer data);
static void _editor_on_find_hide(gpointer data);
static void _editor_on_modified(GtkTextBuffer * tbuf, gpointer data);

/* helpers */
static void _helper_file_dialog_filters(GtkWidget * dialog);


/* public */
/* functions */
/* editor_new */
Editor * editor_new(EditorPrefs * prefs)
{
	Editor * editor;
	GtkAccelGroup * group;
	GtkWidget * vbox;
	GtkWidget * hbox;
	GtkWidget * widget;
	GtkTextBuffer * tbuf;

	if((editor = object_new(sizeof(*editor))) == NULL)
		return NULL;
	if(prefs != NULL)
		editor->prefs = *prefs;
	else
		memset(&editor->prefs, 0, sizeof(editor->prefs));
	editor->config = config_new();
	editor->window = NULL;
	if(editor->config == NULL)
	{
		editor_delete(editor);
		return NULL;
	}
	editor->filename = NULL;
	editor->search = 0;
	editor_config_load(editor);
	/* widgets */
	group = gtk_accel_group_new();
	editor->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_add_accel_group(GTK_WINDOW(editor->window), group);
	g_object_unref(group);
	gtk_window_set_default_size(GTK_WINDOW(editor->window), 600, 400);
	_editor_refresh_title(editor);
#if GTK_CHECK_VERSION(2, 6, 0)
	gtk_window_set_icon_name(GTK_WINDOW(editor->window), "text-editor");
#endif
	g_signal_connect_swapped(G_OBJECT(editor->window), "delete-event",
			G_CALLBACK(on_closex), editor);
	editor->bold = pango_font_description_new();
	pango_font_description_set_weight(editor->bold, PANGO_WEIGHT_BOLD);
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	/* menubar */
#ifndef EMBEDDED
	widget = desktop_menubar_create(editor->prefs.filter
			? _editor_menubar_filter : _editor_menubar, editor,
			group);
	gtk_box_pack_start(GTK_BOX(vbox), widget, FALSE, TRUE, 0);
#else
	desktop_accel_create(_editor_accel, editor, group);
#endif
	/* toolbar */
	widget = desktop_toolbar_create(editor->prefs.filter
			? _editor_toolbar_filter : _editor_toolbar, editor,
			group);
	gtk_box_pack_start(GTK_BOX(vbox), widget, FALSE, FALSE, 0);
#if GTK_CHECK_VERSION(2, 18, 0)
	/* infobar */
	editor->infobar = gtk_info_bar_new_with_buttons(GTK_STOCK_CLOSE,
			GTK_RESPONSE_CLOSE, NULL);
	gtk_info_bar_set_message_type(GTK_INFO_BAR(editor->infobar),
			GTK_MESSAGE_ERROR);
	g_signal_connect(editor->infobar, "close", G_CALLBACK(gtk_widget_hide),
			NULL);
	g_signal_connect(editor->infobar, "response", G_CALLBACK(
				gtk_widget_hide), NULL);
	widget = gtk_info_bar_get_content_area(GTK_INFO_BAR(editor->infobar));
	editor->infobar_label = gtk_label_new(NULL);
	gtk_widget_show(editor->infobar_label);
	gtk_box_pack_start(GTK_BOX(widget), editor->infobar_label, TRUE, TRUE,
			0);
	gtk_widget_set_no_show_all(editor->infobar, TRUE);
	gtk_box_pack_start(GTK_BOX(vbox), editor->infobar, FALSE, TRUE, 0);
#endif
	/* view */
	widget = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(widget),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	editor->view = gtk_text_view_new();
	tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->view));
	g_signal_connect(tbuf, "modified-changed", G_CALLBACK(
				_editor_on_modified), editor);
	editor_set_font(editor, editor_get_font(editor));
	editor_set_wrap_mode(editor, editor_get_wrap_mode(editor));
	gtk_container_add(GTK_CONTAINER(widget), editor->view);
	gtk_box_pack_start(GTK_BOX(vbox), widget, TRUE, TRUE, 0);
	/* find */
	editor->fi_dialog = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	hbox = editor->fi_dialog;
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 4);
	widget = gtk_label_new(_("Find:"));
	gtk_box_pack_start(GTK_BOX(hbox), widget, FALSE, TRUE, 0);
	editor->fi_store = gtk_list_store_new(1, G_TYPE_STRING);
#if GTK_CHECK_VERSION(2, 24, 0)
	editor->fi_text = gtk_combo_box_new_with_model_and_entry(GTK_TREE_MODEL(
				editor->fi_store));
	gtk_combo_box_set_entry_text_column(GTK_COMBO_BOX(editor->fi_text), 0);
#else
	editor->fi_text = gtk_combo_box_entry_new_with_model(GTK_TREE_MODEL(
				editor->fi_store), 0);
#endif
	editor->fi_entry = gtk_bin_get_child(GTK_BIN(editor->fi_text));
	g_signal_connect_swapped(editor->fi_entry, "activate", G_CALLBACK(
				_editor_on_find_clicked), editor);
#if GTK_CHECK_VERSION(2, 16, 0)
	gtk_entry_set_icon_from_stock(GTK_ENTRY(editor->fi_entry),
			GTK_ENTRY_ICON_SECONDARY, GTK_STOCK_CLEAR);
	g_signal_connect_swapped(editor->fi_entry, "icon-release", G_CALLBACK(
				_editor_on_find_clear), editor);
#endif
	gtk_box_pack_start(GTK_BOX(hbox), editor->fi_text, FALSE, TRUE, 0);
	editor->fi_case = gtk_check_button_new_with_label(_("Case-sensitive"));
	gtk_box_pack_start(GTK_BOX(hbox), editor->fi_case, FALSE, TRUE, 0);
	editor->fi_wrap = gtk_check_button_new_with_label(_("Wrap"));
	gtk_box_pack_start(GTK_BOX(hbox), editor->fi_wrap, FALSE, TRUE, 0);
	widget = gtk_button_new_from_stock(GTK_STOCK_FIND);
	g_signal_connect_swapped(widget, "clicked", G_CALLBACK(
				_editor_on_find_clicked), editor);
	gtk_box_pack_start(GTK_BOX(hbox), widget, FALSE, TRUE, 0);
	widget = gtk_button_new();
	gtk_button_set_image(GTK_BUTTON(widget), gtk_image_new_from_stock(
				GTK_STOCK_CLOSE, GTK_ICON_SIZE_BUTTON));
	gtk_button_set_relief(GTK_BUTTON(widget), GTK_RELIEF_NONE);
	g_signal_connect_swapped(widget, "clicked", G_CALLBACK(
				_editor_on_find_hide), editor);
	gtk_box_pack_end(GTK_BOX(hbox), widget, FALSE, TRUE, 0);
	gtk_widget_show_all(hbox);
	gtk_widget_hide(hbox);
	gtk_widget_set_no_show_all(hbox, TRUE);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);
	/* statusbar */
	editor->statusbar = gtk_statusbar_new();
	editor->statusbar_id = 0;
	_editor_set_status(editor, _("Ready"));
	gtk_box_pack_end(GTK_BOX(vbox), editor->statusbar, FALSE, TRUE, 0);
	/* preferences */
	editor->pr_window = NULL;
	/* about */
	editor->ab_window = NULL;
	gtk_container_add(GTK_CONTAINER(editor->window), vbox);
	gtk_window_set_focus(GTK_WINDOW(editor->window), editor->view);
	gtk_widget_show_all(editor->window);
	return editor;
}


/* editor_delete */
void editor_delete(Editor * editor)
{
#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s()\n", __func__);
#endif
	if(editor->window != NULL)
		gtk_widget_destroy(editor->window);
	if(editor->config != NULL)
		config_delete(editor->config);
	pango_font_description_free(editor->bold);
	object_delete(editor);
}


/* accessors */
/* editor_get_font */
char const * editor_get_font(Editor * editor)
{
	char const * p;
	char * q;
	GtkSettings * settings;
	PangoFontDescription * desc;

	if((p = config_get(editor->config, NULL, "font")) != NULL)
		return p;
	settings = gtk_settings_get_default();
	g_object_get(G_OBJECT(settings), "gtk-font-name", &q, NULL);
	if(q != NULL)
	{
		desc = pango_font_description_from_string(q);
		g_free(q);
		pango_font_description_set_family(desc, "monospace");
		q = pango_font_description_to_string(desc);
		config_set(editor->config, NULL, "font", q);
		g_free(q);
		pango_font_description_free(desc);
		if((p = config_get(editor->config, NULL, "font")) != NULL)
			return p;
	}
	return EDITOR_DEFAULT_FONT;
}


/* editor_get_wrap_mode */
GtkWrapMode editor_get_wrap_mode(Editor * editor)
{
	char const * p;
	unsigned int i;

	if((p = config_get(editor->config, NULL, "wrap")) != NULL)
	{
		i = strtoul(p, NULL, 10);
		if(i < sizeof(_editor_wrap) / sizeof(*_editor_wrap))
			return _editor_wrap[i].wrap;
	}
	return GTK_WRAP_WORD_CHAR;
}


/* editor_set_font */
void editor_set_font(Editor * editor, char const * font)
{
	PangoFontDescription * desc;

	desc = pango_font_description_from_string(font);
	gtk_widget_override_font(editor->view, desc);
	pango_font_description_free(desc);
	config_set(editor->config, NULL, "font", font);
}


/* editor_set_wrap_mode */
void editor_set_wrap_mode(Editor * editor, GtkWrapMode wrap)
{
	unsigned int i;
	char buf[10];

	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(editor->view),
			wrap);
	for(i = 0; i < sizeof(_editor_wrap) / sizeof(*_editor_wrap); i++)
		if(_editor_wrap[i].wrap == wrap)
		{
			snprintf(buf, sizeof(buf), "%u", i);
			config_set(editor->config, NULL, "wrap", buf);
			return;
		}
}


/* useful */
/* editor_about */
static gboolean _about_on_closex(gpointer data);

void editor_about(Editor * editor)
{
	if(editor->ab_window != NULL)
	{
		gtk_widget_show(editor->ab_window);
		return;
	}
	editor->ab_window = desktop_about_dialog_new();
	gtk_window_set_transient_for(GTK_WINDOW(editor->ab_window),
			GTK_WINDOW(editor->window));
	desktop_about_dialog_set_authors(editor->ab_window, _authors);
	desktop_about_dialog_set_comments(editor->ab_window,
			_("Text editor for the DeforaOS desktop"));
	desktop_about_dialog_set_copyright(editor->ab_window, _copyright);
	desktop_about_dialog_set_logo_icon_name(editor->ab_window,
			"text-editor");
	desktop_about_dialog_set_license(editor->ab_window, _license);
	desktop_about_dialog_set_name(editor->ab_window, PACKAGE);
	desktop_about_dialog_set_translator_credits(editor->ab_window,
			_("translator-credits"));
	desktop_about_dialog_set_version(editor->ab_window, VERSION);
	desktop_about_dialog_set_website(editor->ab_window,
			"http://www.defora.org/");
	g_signal_connect_swapped(G_OBJECT(editor->ab_window), "delete-event",
			G_CALLBACK(_about_on_closex), editor);
	gtk_widget_show(editor->ab_window);
}

static gboolean _about_on_closex(gpointer data)
{
	Editor * editor = data;

	gtk_widget_hide(editor->ab_window);
	return TRUE;
}


/* editor_config_load */
int editor_config_load(Editor * editor)
{
	int ret;
	char * filename;

	if((filename = _editor_config_filename()) == NULL)
		return -1;
	ret = config_load(editor->config, filename);
	free(filename);
	return ret;
}


/* editor_config_save */
int editor_config_save(Editor * editor)
{
	int ret;
	char * filename;

	if((filename = _editor_config_filename()) == NULL)
		return -1;
	if((ret = config_save(editor->config, filename)) != 0)
		editor_error(editor, _("Could not save the configuration"), 1);
	free(filename);
	return ret;
}


/* editor_confirm */
int editor_confirm(Editor * editor, char const * message, ...)
{
	GtkWidget * dialog;
	va_list ap;
	char const * action;
	int res;

	dialog = gtk_message_dialog_new(GTK_WINDOW(editor->window),
			GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_NONE,
# if GTK_CHECK_VERSION(2, 6, 0)
			"%s", _("Question"));
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
# endif
			"%s", message);
	va_start(ap, message);
	while((action = va_arg(ap, char const *)) != NULL)
		gtk_dialog_add_button(GTK_DIALOG(dialog),
				action, va_arg(ap, int));
	va_end(ap);
	gtk_window_set_title(GTK_WINDOW(dialog), _("Question"));
	res = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return res;
}


/* editor_error */
static int _error_text(char const * message, int ret);

int editor_error(Editor * editor, char const * message, int ret)
{
#if GTK_CHECK_VERSION(2, 18, 0)
	if(editor == NULL)
		return _error_text(message, ret);
	gtk_label_set_text(GTK_LABEL(editor->infobar_label), message);
	gtk_widget_show(editor->infobar);
#else
	GtkWidget * dialog;

	if(editor == NULL)
		return _error_text(message, ret);
	dialog = gtk_message_dialog_new(GTK_WINDOW(editor->window),
			GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
			GTK_BUTTONS_CLOSE,
# if GTK_CHECK_VERSION(2, 6, 0)
			"%s", _("Error"));
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
# endif
			"%s", message);
	gtk_window_set_title(GTK_WINDOW(dialog), _("Error"));
	g_signal_connect(G_OBJECT(dialog), "response", G_CALLBACK(
				gtk_widget_destroy), NULL);
	gtk_widget_show(dialog);
#endif
	return ret;
}

static int _error_text(char const * message, int ret)
{
	fprintf(stderr, "%s: %s\n", PROGNAME, message);
	return ret;
}


/* editor_close */
int editor_close(Editor * editor)
{
	int res;

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s()\n", __func__);
#endif
	if(gtk_text_buffer_get_modified(gtk_text_view_get_buffer(GTK_TEXT_VIEW(
						editor->view))) == FALSE)
	{
		gtk_widget_hide(editor->window);
		gtk_main_quit();
		return 0;
	}
	res = editor_confirm(editor, _("There are unsaved changes.\n"
				"Discard or save them?"),
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_DISCARD, GTK_RESPONSE_REJECT,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
	if(res == GTK_RESPONSE_CANCEL || res == GTK_RESPONSE_DELETE_EVENT)
		return 1;
	else if(res == GTK_RESPONSE_ACCEPT && editor_save(editor) != TRUE)
		return 1;
	gtk_widget_hide(editor->window);
	gtk_main_quit();
	return 0;
}


/* editor_copy */
void editor_copy(Editor * editor)
{
	GtkTextBuffer * buffer;
	GtkClipboard * clipboard;

	if(gtk_window_get_focus(GTK_WINDOW(editor->window))
			== editor->fi_entry)
	{
		gtk_editable_copy_clipboard(GTK_EDITABLE(editor->fi_entry));
		return;
	}
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->view));
	clipboard = gtk_widget_get_clipboard(editor->view,
			GDK_SELECTION_CLIPBOARD);
	gtk_text_buffer_copy_clipboard(buffer, clipboard);
}


/* editor_cut */
void editor_cut(Editor * editor)
{
	GtkTextBuffer * buffer;
	GtkClipboard * clipboard;

	if(gtk_window_get_focus(GTK_WINDOW(editor->window))
			== editor->fi_entry)
	{
		gtk_editable_cut_clipboard(GTK_EDITABLE(editor->fi_entry));
		return;
	}
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->view));
	clipboard = gtk_widget_get_clipboard(editor->view,
			GDK_SELECTION_CLIPBOARD);
	gtk_text_buffer_cut_clipboard(buffer, clipboard, TRUE);
}


/* editor_find */
void editor_find(Editor * editor, char const * text)
{
	gtk_widget_show(editor->fi_dialog);
	if(text != NULL)
		gtk_entry_set_text(GTK_ENTRY(editor->fi_entry), text);
	gtk_widget_grab_focus(editor->fi_entry);
}


/* editor_insert_file */
int editor_insert_file(Editor * editor, char const * filename)
{
	int ret = 0;
	FILE * fp;
	GtkTextBuffer * tbuf;
	char buf[BUFSIZ];
	size_t len;
	char * p;
	size_t rlen;
	size_t wlen;
	GError * error = NULL;

	/* FIXME use a GIOChannel instead (with a GtkDialog or GtkStatusBar) */
	if((fp = fopen(filename, "r")) == NULL)
	{
		snprintf(buf, sizeof(buf), "%s: %s", filename, strerror(errno));
		return -editor_error(editor, buf, 1);
	}
	tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->view));
	while((len = fread(buf, sizeof(char), sizeof(buf), fp)) > 0)
	{
#if 0
		if((p = g_convert(buf, len, "UTF-8", "ISO-8859-15", &rlen, &wlen, NULL)) != NULL)
		{
			gtk_text_buffer_insert_at_cursor(tbuf, p, wlen);
			g_free(p);
		}
		else
			gtk_text_buffer_insert(tbuf, &iter, buf, len);
#else
		if((p = g_locale_to_utf8(buf, len, &rlen, &wlen, &error))
				!= NULL)
			/* FIXME may lose characters */
			gtk_text_buffer_insert_at_cursor(tbuf, p, wlen);
		else
		{
			editor_error(editor, error->message, 1);
			g_error_free(error);
			error = NULL;
			gtk_text_buffer_insert_at_cursor(tbuf, buf, len);
		}
#endif
	}
	if(ferror(fp))
	{
		snprintf(buf, sizeof(buf), "%s: %s", filename, strerror(errno));
		ret = -editor_error(editor, buf, 1);
	}
	fclose(fp);
	return ret;
}


/* editor_insert_file_dialog */
int editor_insert_file_dialog(Editor * editor)
{
	int ret;
	GtkWidget * dialog;
	char * filename = NULL;

	dialog = gtk_file_chooser_dialog_new(_("Insert file..."),
			GTK_WINDOW(editor->window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	_helper_file_dialog_filters(dialog);
	if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(
					dialog));
	gtk_widget_destroy(dialog);
	if(filename == NULL)
		return 0;
	ret = editor_insert_file(editor, filename);
	g_free(filename);
	return ret;
}


/* editor_open */
int editor_open(Editor * editor, char const * filename)
{
	GtkTextBuffer * tbuf;
	int res;
	GtkTextIter iter;

	tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->view));
	if(gtk_text_buffer_get_modified(tbuf) == TRUE)
	{
		res = editor_confirm(editor, _("There are unsaved changes.\n"
					"Discard or save them?"),
				GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
#if GTK_CHECK_VERSION(2, 12, 0)
				GTK_STOCK_DISCARD, GTK_RESPONSE_REJECT,
#else
				_("Discard"), GTK_RESPONSE_REJECT,
#endif
				GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				NULL);
		if(res == GTK_RESPONSE_ACCEPT && editor_save(editor) != TRUE)
			return -1;
		else if(res != GTK_RESPONSE_REJECT)
			return 1;
	}
	gtk_text_buffer_set_text(tbuf, "", 0);
	editor->search = 0;
	if(filename == NULL)
	{
		g_free(editor->filename);
		editor->filename = NULL;
		_editor_refresh_title(editor);
		gtk_text_buffer_set_modified(tbuf, FALSE);
		return 0;
	}
	if((res = editor_insert_file(editor, filename)) != 0)
		return res;
	gtk_text_buffer_set_modified(tbuf, FALSE);
	/* XXX may fail */
	_editor_set_filename(editor, filename);
	_editor_refresh_title(editor);
	/* place the cursor back at the top of the file */
	gtk_text_buffer_get_start_iter(tbuf, &iter);
	gtk_text_buffer_place_cursor(tbuf, &iter);
	return 0;
}


/* editor_open_dialog */
int editor_open_dialog(Editor * editor)
{
	int ret;
	GtkWidget * dialog;
	gchar * filename = NULL;

	dialog = gtk_file_chooser_dialog_new(_("Open file..."),
			GTK_WINDOW(editor->window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	_helper_file_dialog_filters(dialog);
	if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(
					dialog));
	gtk_widget_destroy(dialog);
	if(filename == NULL)
		return 1;
	ret = editor_open(editor, filename);
	g_free(filename);
	return ret;
}


/* editor_paste */
void editor_paste(Editor * editor)
{
	GtkTextBuffer * buffer;
	GtkClipboard * clipboard;

	if(gtk_window_get_focus(GTK_WINDOW(editor->window))
			== editor->fi_entry)
	{
		gtk_editable_paste_clipboard(GTK_EDITABLE(editor->fi_entry));
		return;
	}
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->view));
	clipboard = gtk_widget_get_clipboard(editor->view,
			GDK_SELECTION_CLIPBOARD);
	gtk_text_buffer_paste_clipboard(buffer, clipboard, NULL, TRUE);
}


/* editor_print_dialog */
static void _print_dialog_on_begin_print(GtkPrintOperation * operation,
		GtkPrintContext * context, gpointer data);
static void _print_dialog_on_done(GtkPrintOperation * operation,
		GtkPrintOperationResult result, gpointer data);
static void _print_dialog_on_draw_page(GtkPrintOperation * operation,
		GtkPrintContext * context, gint page, gpointer data);
static void _print_dialog_on_end_print(GtkPrintOperation * operation,
		GtkPrintContext * context, gpointer data);
static gboolean _print_dialog_on_paginate(GtkPrintOperation * operation,
		GtkPrintContext * context, gpointer data);

void editor_print_dialog(Editor * editor)
{
	GtkPrintOperation * operation;
	GtkPrintSettings * settings;
	GError * error = NULL;

	operation = gtk_print_operation_new();
	gtk_print_operation_set_embed_page_setup(operation, TRUE);
	gtk_print_operation_set_unit(operation, GTK_UNIT_POINTS);
	gtk_print_operation_set_use_full_page(operation, FALSE);
	g_signal_connect(operation, "begin-print", G_CALLBACK(
				_print_dialog_on_begin_print), editor);
	g_signal_connect(operation, "done", G_CALLBACK(_print_dialog_on_done),
			editor);
	g_signal_connect(operation, "draw-page", G_CALLBACK(
				_print_dialog_on_draw_page), editor);
	g_signal_connect(operation, "end-print", G_CALLBACK(
				_print_dialog_on_end_print), editor);
	g_signal_connect(operation, "paginate", G_CALLBACK(
				_print_dialog_on_paginate), editor);
	settings = gtk_print_settings_new();
	gtk_print_operation_set_print_settings(operation, settings);
	gtk_print_operation_run(operation,
			GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,
			GTK_WINDOW(editor->window), &error);
	g_object_unref(settings);
	g_object_unref(operation);
	if(error)
	{
		editor_error(editor, error->message, 1);
		g_error_free(error);
	}
}

static void _print_dialog_on_begin_print(GtkPrintOperation * operation,
		GtkPrintContext * context, gpointer data)
{
	const gint size = 10;
	Editor * editor = data;
	char const * font;

	/* initialize the font */
	font = editor_get_font(editor);
	editor->font = pango_font_description_from_string(font);
	pango_font_description_set_size(editor->font,
			pango_units_from_double(size));
	editor->font_size = size;
	editor->line_space = 0.0;
}

static void _print_dialog_on_done(GtkPrintOperation * operation,
		GtkPrintOperationResult result, gpointer data)
{
	Editor * editor = data;
	GError * error = NULL;

	switch(result)
	{
		case GTK_PRINT_OPERATION_RESULT_ERROR:
			gtk_print_operation_get_error(operation, &error);
			editor_error(editor, error->message, 2);
			g_error_free(error);
			break;
		default:
			break;
	}
}

static void _print_dialog_on_draw_page(GtkPrintOperation * operation,
		GtkPrintContext * context, gint page, gpointer data)
{
	Editor * editor = data;
	GtkTextBuffer * tbuf;
	cairo_t * cairo;
	PangoLayout * layout;
	guint i;
	gboolean valid = TRUE;
	GtkTextIter end;
	gchar * p;

	tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->view));
	cairo = gtk_print_context_get_cairo_context(context);
	layout = gtk_print_context_create_pango_layout(context);
	/* set the font */
	pango_layout_set_font_description(layout, editor->font);
	/* print the text */
	cairo_move_to(cairo, 0.0, 0.0);
	gtk_text_buffer_get_iter_at_line(tbuf, &editor->iter,
			editor->line_count * page);
	for(i = 0, valid = !gtk_text_iter_is_end(&editor->iter);
			i < editor->line_count && valid == TRUE;
			i++, valid = gtk_text_iter_forward_line(&editor->iter))
	{
		end = editor->iter;
		if(!gtk_text_iter_ends_line(&end))
			gtk_text_iter_forward_to_line_end(&end);
		p = gtk_text_buffer_get_text(tbuf, &editor->iter, &end, FALSE);
		/* FIXME the line may be too long */
		pango_layout_set_text(layout, p, -1);
		g_free(p);
		pango_cairo_show_layout(cairo, layout);
		cairo_rel_move_to(cairo, 0.0, editor->font_size
				+ editor->line_space);
	}
	g_object_unref(layout);
}

static void _print_dialog_on_end_print(GtkPrintOperation * operation,
		GtkPrintContext * context, gpointer data)
{
	Editor * editor = data;

	pango_font_description_free(editor->font);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(editor->view), TRUE);
}

static gboolean _print_dialog_on_paginate(GtkPrintOperation * operation,
		GtkPrintContext * context, gpointer data)
{
	Editor * editor = data;
	GtkTextBuffer * tbuf;
	gint count;
	double height;

	/* count the lines to print */
	gtk_text_view_set_editable(GTK_TEXT_VIEW(editor->view), FALSE);
	tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->view));
	count = gtk_text_buffer_get_line_count(tbuf);
	/* count the pages required */
	height = gtk_print_context_get_height(context);
	editor->line_count = floor(height / (editor->font_size
				+ editor->line_space));
	gtk_print_operation_set_n_pages(operation,
			((count - 1) / editor->line_count) + 1);
	return TRUE;
}


/* editor_save */
gboolean editor_save(Editor * editor)
{
	FILE * fp;
	GtkTextBuffer * tbuf;
	GtkTextIter start;
	GtkTextIter end;
	gchar * buf;
	size_t len;

	if(editor->filename == NULL)
		return editor_save_as_dialog(editor);
	if((fp = fopen(editor->filename, "w")) == NULL)
	{
		buf = g_strdup_printf("%s: %s", editor->filename, strerror(
					errno));
		editor_error(editor, buf, 1);
		g_free(buf);
		return FALSE;
	}
	tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->view));
	/* XXX allocating the complete file is not optimal */
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(tbuf), &start);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(tbuf), &end);
	buf = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(tbuf), &start, &end,
			FALSE);
	len = strlen(buf);
	if(fwrite(buf, sizeof(*buf), len, fp) != len)
	{
		g_free(buf);
		fclose(fp);
		editor_error(editor, _("Partial write"), 1);
		return FALSE;
	}
	g_free(buf);
	if(fclose(fp) != 0)
	{
		editor_error(editor, _("Partial write"), 1);
		return FALSE;
	}
	gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(tbuf), FALSE);
	return TRUE;
}


/* editor_save_as */
gboolean editor_save_as(Editor * editor, char const * filename)
{
	struct stat st;
	GtkWidget * dialog;
	int res;

	if(stat(filename, &st) == 0)
	{
		dialog = gtk_message_dialog_new(GTK_WINDOW(editor->window),
				GTK_DIALOG_MODAL
				| GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
#if GTK_CHECK_VERSION(2, 6, 0)
				"%s", _("Question"));
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(
					dialog),
#endif
				"%s",
				_("This file already exists. Overwrite?"));
		gtk_window_set_title(GTK_WINDOW(dialog), _("Question"));
		res = gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		if(res == GTK_RESPONSE_NO)
			return FALSE;
	}
	if(_editor_set_filename(editor, filename) != 0
		|| editor_save(editor) != TRUE)
		return FALSE;
	_editor_refresh_title(editor);
	return TRUE;
}


/* editor_save_as_dialog */
gboolean editor_save_as_dialog(Editor * editor)
{
	gboolean ret;
	GtkWidget * dialog;
	gchar * filename = NULL;

	dialog = gtk_file_chooser_dialog_new(_("Save as..."),
			GTK_WINDOW(editor->window),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
	if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(
					dialog));
	gtk_widget_destroy(dialog);
	if(filename == NULL)
		return FALSE;
	ret = editor_save_as(editor, filename);
	g_free(filename);
	return ret;
}


/* editor_select_all */
void editor_select_all(Editor * editor)
{
	GtkTextBuffer * tbuf;
	GtkTextIter start;
	GtkTextIter end;

	tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->view));
	gtk_text_buffer_get_start_iter(tbuf, &start);
	gtk_text_buffer_get_end_iter(tbuf, &end);
	gtk_text_buffer_select_range(tbuf, &start, &end);
}


/* editor_show_preferences */
static gboolean _preferences_on_closex(gpointer data);
static void _preferences_on_response(GtkWidget * widget, gint response,
		gpointer data);
static void _preferences_on_apply(gpointer data);
static void _preferences_on_cancel(gpointer data);
static void _preferences_on_ok(gpointer data);

void editor_show_preferences(Editor * editor, gboolean show)
{
	GtkWidget * vbox;
	GtkWidget * hbox;
	GtkWidget * widget;
	GtkSizeGroup * group;
	size_t i;

	if(editor->pr_window != NULL)
	{
		if(show)
			gtk_window_present(GTK_WINDOW(editor->pr_window));
		else
			gtk_widget_hide(editor->pr_window);
		return;
	}
	editor->pr_window = gtk_dialog_new_with_buttons(
			_("Text editor preferences"),
			GTK_WINDOW(editor->window),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_APPLY, GTK_RESPONSE_APPLY,
			GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
	gtk_window_set_resizable(GTK_WINDOW(editor->pr_window), FALSE);
	g_signal_connect_swapped(G_OBJECT(editor->pr_window), "delete-event",
			G_CALLBACK(_preferences_on_closex), editor);
	g_signal_connect(G_OBJECT(editor->pr_window), "response",
			G_CALLBACK(_preferences_on_response), editor);
#if GTK_CHECK_VERSION(2, 14, 0)
	vbox = gtk_dialog_get_content_area(GTK_DIALOG(editor->pr_window));
#else
	vbox = GTK_DIALOG(editor->pr_window)->vbox;
#endif
	gtk_box_set_spacing(GTK_BOX(vbox), 4);
	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	/* font */
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	widget = gtk_label_new(_("Font:"));
#if GTK_CHECK_VERSION(3, 0, 0)
	g_object_set(widget, "halign", GTK_ALIGN_START, NULL);
#else
	gtk_misc_set_alignment(GTK_MISC(widget), 0.0, 0.5);
#endif
	gtk_size_group_add_widget(group, widget);
	gtk_box_pack_start(GTK_BOX(hbox), widget, FALSE, TRUE, 0);
	editor->pr_font = gtk_font_button_new();
	gtk_font_button_set_use_font(GTK_FONT_BUTTON(editor->pr_font), TRUE);
	gtk_box_pack_start(GTK_BOX(hbox), editor->pr_font, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	/* wrap mode */
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	widget = gtk_label_new(_("Wrap mode:"));
#if GTK_CHECK_VERSION(3, 0, 0)
	g_object_set(widget, "halign", GTK_ALIGN_START, NULL);
#else
	gtk_misc_set_alignment(GTK_MISC(widget), 0.0, 0.5);
#endif
	gtk_size_group_add_widget(group, widget);
	gtk_box_pack_start(GTK_BOX(hbox), widget, FALSE, TRUE, 0);
#if GTK_CHECK_VERSION(2, 24, 0)
	editor->pr_wrap = gtk_combo_box_text_new();
#else
	editor->pr_wrap = gtk_combo_box_new_text();
#endif
	for(i = 0; i < sizeof(_editor_wrap) / sizeof(*_editor_wrap); i++)
#if GTK_CHECK_VERSION(2, 24, 0)
		gtk_combo_box_text_append_text(
				GTK_COMBO_BOX_TEXT(editor->pr_wrap),
				_(_editor_wrap[i].name));
#else
		gtk_combo_box_append_text(GTK_COMBO_BOX(editor->pr_wrap),
				_(_editor_wrap[i].name));
#endif
	gtk_box_pack_start(GTK_BOX(hbox), editor->pr_wrap, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	_preferences_on_cancel(editor);
	gtk_widget_show_all(vbox);
	if(show)
		gtk_widget_show(editor->pr_window);
}

static gboolean _preferences_on_closex(gpointer data)
{
	Editor * editor = data;

	_preferences_on_cancel(editor);
	return TRUE;
}

static void _preferences_on_response(GtkWidget * widget, gint response,
		gpointer data)
{
	Editor * editor = data;

	if(response == GTK_RESPONSE_OK)
		_preferences_on_ok(editor);
	else if(response == GTK_RESPONSE_APPLY)
		_preferences_on_apply(editor);
	else if(response == GTK_RESPONSE_CANCEL)
		_preferences_on_cancel(editor);
}

static void _preferences_on_apply(gpointer data)
{
	Editor * editor = data;
	char const * font;
	size_t i;

	font = gtk_font_button_get_font_name(GTK_FONT_BUTTON(editor->pr_font));
	editor_set_font(editor, font);
	i = gtk_combo_box_get_active(GTK_COMBO_BOX(editor->pr_wrap));
	if(i < sizeof(_editor_wrap) / sizeof(*_editor_wrap))
		editor_set_wrap_mode(editor, _editor_wrap[i].wrap);
}

static void _preferences_on_cancel(gpointer data)
{
	Editor * editor = data;
	char const * p;
	gint u = 0;

	gtk_widget_hide(editor->pr_window);
	gtk_font_button_set_font_name(GTK_FONT_BUTTON(editor->pr_font),
			editor_get_font(editor));
	if((p = config_get(editor->config, NULL, "wrap")) != NULL)
		u = strtol(p, NULL, 10);
	gtk_combo_box_set_active(GTK_COMBO_BOX(editor->pr_wrap), u);
}

static void _preferences_on_ok(gpointer data)
{
	Editor * editor = data;

	gtk_widget_hide(editor->pr_window);
	_preferences_on_apply(editor);
	editor_config_save(editor);
}


/* editor_show_properties */
static GtkWidget * _properties_widget(Editor * editor, GtkSizeGroup * group,
		char const * label, GtkWidget * value);

void editor_show_properties(Editor * editor, gboolean show)
{
	GtkWidget * dialog;
	GtkSizeGroup * hgroup;
	GtkSizeGroup * vgroup;
	GtkWidget * vbox;
	GtkWidget * widget;
	GtkTextBuffer * tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(
				editor->view));
	gchar * p;
	gchar * q;
	char buf[256];
	GError * error = NULL;

	if(show == FALSE)
		/* XXX should really hide the window */
		return;
	p = (editor->filename != NULL)
		? g_filename_display_basename(editor->filename) : NULL;
	if(p != NULL)
		snprintf(buf, sizeof(buf), _("Properties of %s"), p);
	else
		snprintf(buf, sizeof(buf), "%s", _("Properties"));
	g_free(p);
	dialog = gtk_message_dialog_new(GTK_WINDOW(editor->window),
			GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE,
#if GTK_CHECK_VERSION(2, 6, 0)
			"%s", _("Properties"));
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
#endif
			"");
#if GTK_CHECK_VERSION(2, 10, 0)
	gtk_message_dialog_set_image(GTK_MESSAGE_DIALOG(dialog),
			gtk_image_new_from_stock(GTK_STOCK_PROPERTIES,
				GTK_ICON_SIZE_DIALOG));
#endif
	gtk_window_set_title(GTK_WINDOW(dialog), _("Properties"));
	hgroup = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	vgroup = gtk_size_group_new(GTK_SIZE_GROUP_VERTICAL);
#if GTK_CHECK_VERSION(2, 14, 0)
	vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
#else
	vbox = dialog->vbox;
#endif
	/* filename */
	p = g_strdup((editor->filename != NULL) ? editor->filename : "");
	if((q = g_filename_to_utf8(p, -1, NULL, NULL, &error)) == NULL)
	{
		editor_error(NULL, error->message, 1);
		g_error_free(error);
		q = p;
	}
	widget = gtk_entry_new();
	gtk_editable_set_editable(GTK_EDITABLE(widget), FALSE);
	gtk_entry_set_text(GTK_ENTRY(widget), q);
	gtk_size_group_add_widget(vgroup, widget);
	g_free(p);
	widget = _properties_widget(editor, hgroup, _("Filename:"), widget);
	gtk_box_pack_start(GTK_BOX(vbox), widget, FALSE, FALSE, 0);
	/* characters */
	snprintf(buf, sizeof(buf), "%u", gtk_text_buffer_get_char_count(tbuf));
	widget = gtk_label_new(buf);
#if GTK_CHECK_VERSION(3, 0, 0)
	g_object_set(widget, "halign", GTK_ALIGN_START, NULL);
#else
	gtk_misc_set_alignment(GTK_MISC(widget), 0.0, 0.5);
#endif
	gtk_size_group_add_widget(vgroup, widget);
	widget = _properties_widget(editor, hgroup, _("Characters:"), widget);
	gtk_box_pack_start(GTK_BOX(vbox), widget, FALSE, FALSE, 0);
	/* lines */
	snprintf(buf, sizeof(buf), "%u", gtk_text_buffer_get_line_count(tbuf));
	widget = gtk_label_new(buf);
#if GTK_CHECK_VERSION(3, 0, 0)
	g_object_set(widget, "halign", GTK_ALIGN_START, NULL);
#else
	gtk_misc_set_alignment(GTK_MISC(widget), 0.0, 0.5);
#endif
	gtk_size_group_add_widget(vgroup, widget);
	widget = _properties_widget(editor, hgroup, _("Lines:"), widget);
	gtk_box_pack_start(GTK_BOX(vbox), widget, FALSE, FALSE, 0);
	/* FIXME implement more properties */
	gtk_widget_show_all(vbox);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

static GtkWidget * _properties_widget(Editor * editor, GtkSizeGroup * group,
		char const * label, GtkWidget * value)
{
	GtkWidget * hbox;
	GtkWidget * widget;

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	widget = gtk_label_new(label);
	gtk_widget_override_font(widget, editor->bold);
#if GTK_CHECK_VERSION(3, 0, 0)
	g_object_set(widget, "halign", GTK_ALIGN_START, NULL);
#else
	gtk_misc_set_alignment(GTK_MISC(widget), 0.0, 0.5);
#endif
	gtk_size_group_add_widget(group, widget);
	gtk_box_pack_start(GTK_BOX(hbox), widget, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), value, TRUE, TRUE, 0);
	return hbox;
}


/* editor_unselect_all */
void editor_unselect_all(Editor * editor)
{
	GtkTextBuffer * tbuf;
	GtkTextMark * mark;
	GtkTextIter iter;

	tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->view));
	mark = gtk_text_buffer_get_mark(tbuf, "insert");
	gtk_text_buffer_get_iter_at_mark(tbuf, &iter, mark);
	gtk_text_buffer_select_range(tbuf, &iter, &iter);
}


/* private */
/* functions */
/* accessors */
/* editor_set_filename */
static int _editor_set_filename(Editor * editor, char const * filename)
{
	gchar * p = NULL;
	char * q;

	if(g_path_is_absolute(filename))
		p = g_strdup(filename);
	else if((q = getcwd(NULL, 0)) != NULL)
	{
		p = g_build_filename(q, filename, NULL);
		free(q);
	}
	if(p == NULL)
		return -editor_error(editor, _("Could not update the filename"),
				1);
	g_free(editor->filename);
	editor->filename = p;
	return 0;
}


/* editor_set_status */
static void _editor_set_status(Editor * editor, char const * status)
{
	GtkStatusbar * sb = GTK_STATUSBAR(editor->statusbar);

	if(editor->statusbar_id != 0)
		gtk_statusbar_remove(sb, gtk_statusbar_get_context_id(sb, ""),
				editor->statusbar_id);
	editor->statusbar_id = gtk_statusbar_push(sb,
			gtk_statusbar_get_context_id(sb, ""), status);
}


/* editor_config_filename */
static char * _editor_config_filename(void)
{
	char const * homedir;
	size_t len;
	char * filename;

	if((homedir = getenv("HOME")) == NULL)
		homedir = g_get_home_dir();
	len = strlen(homedir) + 1 + sizeof(EDITOR_CONFIG_FILE);
	if((filename = malloc(len)) == NULL)
		return NULL;
	snprintf(filename, len, "%s/%s", homedir, EDITOR_CONFIG_FILE);
	return filename;
}


/* editor_find */
static char const * _find_string(char const * big, char const * little,
		gboolean sensitive);
static gboolean _find_match(Editor * editor, GtkTextBuffer * buffer,
		char const * buf, char const * str, size_t len);

static gboolean _editor_find(Editor * editor, char const * text,
		gboolean sensitive, gboolean wrap)
{
	gboolean ret = FALSE;
	size_t tlen;
	GtkTextBuffer * buffer;
	GtkTextIter start;
	GtkTextIter end;
	gchar * buf;
	size_t blen;
	char const * str;

	if(text == NULL || (tlen = strlen(text)) == 0)
		return ret;
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->view));
	/* XXX highly inefficient */
	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);
	buf = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
	if(buf == NULL || (blen = strlen(buf)) == 0)
		return ret;
	if(editor->search >= blen)
		editor->search = 0;
	if((str = _find_string(&buf[editor->search], text, sensitive)) != NULL)
		ret = _find_match(editor, buffer, buf, str, tlen);
	else if(wrap && editor->search != 0) /* wrap around */
	{
		buf[editor->search] = '\0';
		if((str = _find_string(buf, text, sensitive)) != NULL)
			ret = _find_match(editor, buffer, buf, str, tlen);
	}
	g_free(buf);
	return ret;
}

static char const * _find_string(char const * big, char const * little,
		gboolean sensitive)
{
	return sensitive ? strstr(big, little) : strcasestr(big, little);
}

static gboolean _find_match(Editor * editor, GtkTextBuffer * buffer,
		char const * buf, char const * str, size_t len)
{
	size_t offset;
	GtkTextIter start;
	GtkTextIter end;

	offset = str - buf;
	editor->search = offset + 1;
	gtk_text_buffer_get_iter_at_offset(buffer, &start, offset);
	gtk_text_buffer_get_iter_at_offset(buffer, &end, offset + len);
	gtk_text_buffer_select_range(buffer, &start, &end);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(editor->view), &start, 0.0,
			FALSE, 0.0, 0.0);
	return TRUE;
}


/* editor_refresh_title */
static void _editor_refresh_title(Editor * editor)
{
	char buf[256];

	snprintf(buf, sizeof(buf), "%s%s", _("Text editor - "),
			(editor->filename == NULL)
			? _("(Untitled)") : editor->filename);
	gtk_window_set_title(GTK_WINDOW(editor->window), buf);
}


/* callbacks */
#if GTK_CHECK_VERSION(2, 16, 0)
/* editor_on_find_clear */
static void _editor_on_find_clear(gpointer data)
{
	Editor * editor = data;

	gtk_entry_set_text(GTK_ENTRY(editor->fi_entry), "");
}
#endif


/* editor_on_find_clicked */
static void _editor_on_find_clicked(gpointer data)
{
	Editor * editor = data;
	char const * text;
	GtkTreeModel * model = GTK_TREE_MODEL(editor->fi_store);
	GtkTreeIter iter;
	gboolean valid;
	char * p;
	int res;
	gboolean sensitive;
	gboolean wrap;

	if((text = gtk_entry_get_text(GTK_ENTRY(editor->fi_entry))) == NULL
			|| strlen(text) == 0)
		return;
	/* only append the text currently searched if not already known */
	for(valid = gtk_tree_model_get_iter_first(model, &iter); valid == TRUE;
			valid = gtk_tree_model_iter_next(model, &iter))
	{
		gtk_tree_model_get(model, &iter, 0, &p, -1);
		res = strcmp(text, p);
		free(p);
		if(res == 0)
			break;
	}
	if(valid == FALSE)
	{
		gtk_list_store_append(editor->fi_store, &iter);
		gtk_list_store_set(editor->fi_store, &iter, 0, text, -1);
	}
	sensitive = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
				editor->fi_case));
	wrap = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
				editor->fi_wrap));
	if(_editor_find(editor, text, sensitive, wrap) == TRUE)
		return;
	editor_error(editor, _("Text not found"), 0);
}


/* editor_on_find_hide */
static void _editor_on_find_hide(gpointer data)
{
	Editor * editor = data;

	gtk_widget_hide(editor->fi_dialog);
}


/* editor_on_modified */
static void _editor_on_modified(GtkTextBuffer * tbuf, gpointer data)
{
	Editor * editor = data;
	gboolean modified;
	char const * status;

	if((modified = gtk_text_buffer_get_modified(tbuf)) == TRUE)
		status = _("Unsaved changes");
	else if(editor->filename != NULL)
		status = _("Saved");
	else
		status = _("Ready");
	_editor_set_status(editor, status);
}


/* helpers */
/* helper_file_dialog_filters */
static void _helper_file_dialog_filters(GtkWidget * dialog)
{
	GtkFileFilter * filter;
	char const * types[] = {
		"application/x-perl",
		"application/x-shellscript",
		"application/xml",
		"application/xslt+xml",
		"text/plain" };
	size_t i;

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Text files"));
	for(i = 0; i < sizeof(types) / sizeof(*types); i++)
		gtk_file_filter_add_mime_type(filter, types[i]);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
}
