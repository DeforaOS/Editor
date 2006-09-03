/* editor.c */



#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "../config.h"


/* constants */
static char const * _authors[] =
{
	"Pierre 'khorben' Pronchery",
	NULL
};
static char const _license[] = "GPLv2";

/* Editor */
static GtkWidget * _new_menubar(Editor * editor);
/* callbacks */
static void _on_close(GtkWidget * widget, gpointer data);
static gboolean _on_closex(GtkWidget * widget, GdkEvent * event, gpointer data);
static void _on_edit_preferences(GtkWidget * widget, gpointer data);
static void _on_file_close(GtkWidget * widget, gpointer data);
static void _on_file_new(GtkWidget * widget, gpointer data);
static void _on_file_open(GtkWidget * widget, gpointer data);
static void _on_file_save(GtkWidget * widget, gpointer data);
static void _on_file_save_as(GtkWidget * widget, gpointer data);
static void _on_help_about(GtkWidget * widget, gpointer data);
static void _on_new(GtkWidget * widget, gpointer data);
static void _on_open(GtkWidget * widget, gpointer data);
struct _menu
{
	char * name;
	GtkSignalFunc callback;
	char * stock;
};
struct _menubar
{
	char * name;
	struct _menu * menu;
};
struct _menu _menu_file[] =
{
	{ "_New", G_CALLBACK(_on_file_new), GTK_STOCK_NEW },
	{ "_Open", G_CALLBACK(_on_file_open), GTK_STOCK_OPEN },
	{ "", NULL, NULL },
	{ "_Save", G_CALLBACK(_on_file_save), GTK_STOCK_SAVE },
	{ "_Save as...", G_CALLBACK(_on_file_save_as), GTK_STOCK_SAVE_AS },
	{ "", NULL, NULL },
	{ "_Close", G_CALLBACK(_on_file_close), GTK_STOCK_CLOSE },
	{ NULL, NULL, NULL }
};
struct _menu _menu_edit[] =
{
	{ "_Cut", NULL, GTK_STOCK_CUT },
	{ "_Copy", NULL, GTK_STOCK_COPY },
	{ "_Paste", NULL, GTK_STOCK_PASTE },
	{ "", NULL, NULL },
	{ "_Preferences", G_CALLBACK(_on_edit_preferences),
		GTK_STOCK_PREFERENCES },
	{ NULL, NULL, NULL }
};
struct _menu _menu_help[] =
{
#if GTK_CHECK_VERSION(2, 6, 0)
	{ "_About", G_CALLBACK(_on_help_about), GTK_STOCK_ABOUT },
#else
	{ "_About", G_CALLBACK(_on_help_about), NULL },
#endif
	{ NULL, NULL, NULL }
};
static struct _menubar _menubar[] =
{
	{ "_File", _menu_file },
	{ "_Edit", _menu_edit },
	{ "_Help", _menu_help },
	{ NULL, NULL }
};
Editor * editor_new(void)
{
	Editor * editor;
	GtkWidget * vbox;
	GtkWidget * toolbar;
	GtkToolItem * tb_button;
	GtkWidget * widget;

	if((editor = malloc(sizeof(*editor))) == NULL)
		return NULL;
	editor->filename = NULL;
	/* widgets */
	editor->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(editor->window), 512, 384);
	gtk_window_set_title(GTK_WINDOW(editor->window), "Text editor");
	g_signal_connect(G_OBJECT(editor->window), "delete_event", G_CALLBACK(
			_on_closex), editor);
	vbox = gtk_vbox_new(FALSE, 0);
	/* menubar */
	gtk_box_pack_start(GTK_BOX(vbox), _new_menubar(editor), FALSE, FALSE,
			0);
	/* toolbar */
	toolbar = gtk_toolbar_new();
	tb_button = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
	g_signal_connect(G_OBJECT(tb_button), "clicked", G_CALLBACK(_on_new),
			editor);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tb_button, -1);
	tb_button = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
	g_signal_connect(G_OBJECT(tb_button), "clicked", G_CALLBACK(_on_open),
			editor);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tb_button, -1);
	tb_button = gtk_tool_button_new_from_stock(GTK_STOCK_CLOSE);
	g_signal_connect(G_OBJECT(tb_button), "clicked", G_CALLBACK(_on_close),
			editor);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tb_button, -1);
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
	/* view */
	widget = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(widget),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	editor->view = gtk_text_view_new();
	gtk_container_add(GTK_CONTAINER(widget), editor->view);
	gtk_box_pack_start(GTK_BOX(vbox), widget, TRUE, TRUE, 0);
	/* statusbar */
	editor->statusbar = gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX(vbox), editor->statusbar, FALSE, FALSE, 0);
	/* preferences */
	editor->pr_window = NULL;
	gtk_container_add(GTK_CONTAINER(editor->window), vbox);
	gtk_widget_show_all(editor->window);
	return editor;
}

static GtkWidget * _new_menubar(Editor * editor)
{
	GtkWidget * tb_menubar;
	GtkWidget * menu;
	GtkWidget * menubar;
	GtkWidget * menuitem;
	unsigned int i;
	unsigned int j;
	struct _menu * p;

	tb_menubar = gtk_menu_bar_new();
	for(i = 0; _menubar[i].name != NULL; i++)
	{
		menubar = gtk_menu_item_new_with_mnemonic(_menubar[i].name);
		menu = gtk_menu_new();
		for(j = 0; _menubar[i].menu[j].name != NULL; j++)
		{
			p = &_menubar[i].menu[j];
			if(p->name[0] == '\0')
				menuitem = gtk_separator_menu_item_new();
			else if(p->stock == 0)
				menuitem = gtk_menu_item_new_with_mnemonic(
						p->name);
			else
				menuitem = gtk_image_menu_item_new_from_stock(
						p->stock, NULL);
			if(p->callback != NULL)
				g_signal_connect(G_OBJECT(menuitem), "activate",
						G_CALLBACK(p->callback), editor);
			else
				gtk_widget_set_sensitive(menuitem, FALSE);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
		}
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menubar), menu);
		gtk_menu_bar_append(GTK_MENU_BAR(tb_menubar), menubar);
	}
	return tb_menubar;
}

static int _editor_error(Editor * editor, char const * message, int ret)
{
	GtkWidget * dialog;

	dialog = gtk_message_dialog_new(GTK_WINDOW(editor->window),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", message);
	g_signal_connect(G_OBJECT(dialog), "response", G_CALLBACK(
				gtk_widget_destroy), NULL);
	gtk_widget_show(dialog);
	return ret;
}

static gboolean _on_closex(GtkWidget * widget, GdkEvent * event,
		gpointer data)
{
	Editor * editor = data;

	return editor_close(editor);
}

static void _on_close(GtkWidget * widget, gpointer data)
{
	Editor * editor = data;

	editor_close(editor);
}

static void _preferences_set(Editor * editor);
/* callbacks */
static void _preferences_on_cancel(GtkWidget * widget, gpointer data);
static gboolean _preferences_on_close(GtkWidget * widget, GdkEvent * event,
		gpointer data);
static void _preferences_on_ok(GtkWidget * widget, gpointer data);
static void _on_edit_preferences(GtkWidget * widget, gpointer data)
{
	Editor * editor = data;
	GtkWidget * vbox;
	GtkWidget * hbox;
	GtkSizeGroup * group;

	if(editor->pr_window != NULL)
	{
		gtk_widget_show(editor->pr_window);
		return;
	}
	editor->pr_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_resizable(GTK_WINDOW(editor->pr_window), FALSE);
	gtk_window_set_title(GTK_WINDOW(editor->pr_window),
			"Text editor preferences");
	gtk_window_set_transient_for(GTK_WINDOW(editor->pr_window), GTK_WINDOW(
				editor->window));
	g_signal_connect(G_OBJECT(editor->pr_window), "delete_event",
			G_CALLBACK(_preferences_on_close), editor);
	vbox = gtk_vbox_new(FALSE, 0);
	/* dialog */
	hbox = gtk_hbox_new(FALSE, 0);
	group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	widget = gtk_button_new_from_stock(GTK_STOCK_OK);
	gtk_size_group_add_widget(group, widget);
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(
				_preferences_on_ok), editor);
	gtk_box_pack_end(GTK_BOX(hbox), widget, FALSE, TRUE, 4);
	widget = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_size_group_add_widget(group, widget);
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(
				_preferences_on_cancel), editor);
	gtk_box_pack_end(GTK_BOX(hbox), widget, FALSE, TRUE, 4);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 4);
	gtk_container_add(GTK_CONTAINER(editor->pr_window), vbox);
	_preferences_set(editor);
	gtk_widget_show_all(editor->pr_window);
}

static void _preferences_set(Editor * editor)
{
}

static void _preferences_on_cancel(GtkWidget * widget, gpointer data)
{
	Editor * editor = data;

	gtk_widget_hide(editor->pr_window);
	_preferences_set(editor);
}

static gboolean _preferences_on_close(GtkWidget * widget, GdkEvent * event,
		gpointer data)
{
	Editor * editor = data;

	_preferences_on_cancel(widget, editor);
	return FALSE;
}

static void _preferences_on_ok(GtkWidget * widget, gpointer data)
{
	Editor * editor = data;

	gtk_widget_hide(editor->pr_window);
	/* FIXME apply settings */
}

static void _on_file_close(GtkWidget * widget, gpointer data)
{
	Editor * editor = data;

	/* FIXME ask the user whether to save or not */
	gtk_main_quit();
}

static void _on_file_new(GtkWidget * widget, gpointer data)
{
	Editor * editor = data;

	editor_open(editor, NULL);
}

static void _on_file_open(GtkWidget * widget, gpointer data)
{
	Editor * editor = data;

	editor_open_dialog(editor);
}

static void _on_file_save(GtkWidget * widget, gpointer data)
{
}

static void _on_file_save_as(GtkWidget * widget, gpointer data)
{
}

/* callbacks */
static gboolean _about_on_closex(GtkWidget * widget, GdkEvent * event,
		gpointer data);
#if !GTK_CHECK_VERSION(2, 6, 0)
static void _about_on_close(GtkWidget * widget, gpointer data);
static void _about_on_credits(GtkWidget * widget, gpointer data);
static void _about_on_license(GtkWidget * widget, gpointer data);
#endif
static void _on_help_about(GtkWidget * widget, gpointer data)
{
	Editor * editor = data;
	static GtkWidget * window = NULL;
	char const copyright[] = "Copyright (c) 2006 khorben";
#if GTK_CHECK_VERSION(2, 6, 0)
	gsize cnt = 65536;
	gchar * buf;

	if(window != NULL)
	{
		gtk_widget_show(window);
		return;
	}
	if((buf = malloc(sizeof(*buf) * cnt)) == NULL)
	{
		_editor_error(editor, "malloc", 0);
		return;
	}
	window = gtk_about_dialog_new();
	gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(
				editor->window));
	g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(
				_about_on_closex), NULL);
	gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(window), PACKAGE);
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(window), VERSION);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(window), copyright);
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(window), _authors);
	if(g_file_get_contents("/usr/share/common-licenses/GPL-2", &buf, &cnt,
				NULL) == TRUE)
		gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(window), buf);
	else
		gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(window),
				_license);
	free(buf);
	gtk_widget_show(window);
}
#else
	GtkWidget * vbox;
	GtkWidget * hbox;
	GtkWidget * button;

	if(window != NULL)
	{
		gtk_widget_show(window);
		return;
	}
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 4);
	gtk_window_set_title(GTK_WINDOW(window), "About Editor");
	gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(
				editor->window));
	g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(
				_about_on_closex), window);
	vbox = gtk_vbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new(PACKAGE " " VERSION),
			FALSE, FALSE, 2); 
	gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new(copyright), FALSE,
			FALSE, 2);
	hbox = gtk_hbox_new(TRUE, 4);
	button = gtk_button_new_with_mnemonic("C_redits");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(
				_about_on_credits), window);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 4);
	button = gtk_button_new_with_mnemonic("_License");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(
				_about_on_license), window);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, TRUE, 4);
	button = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(
				_about_on_close), window);
	gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, TRUE, 4);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 4);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show_all(window);
}
#endif /* !GTK_CHECK_VERSION(2, 6, 0) */

static gboolean _about_on_closex(GtkWidget * widget, GdkEvent * event,
		gpointer data)
{
	gtk_widget_hide(widget);
	return TRUE;
}

#if !GTK_CHECK_VERSION(2, 6, 0)
static void _about_on_close(GtkWidget * widget, gpointer data)
{
        GtkWidget * window = data;

        gtk_widget_hide(window);
}

static void _about_on_credits(GtkWidget * widget, gpointer data)
{
	static GtkWidget * window = NULL;
	GtkWidget * about = data;
	GtkWidget * vbox;
	GtkWidget * notebook;
	GtkWidget * textview;
	GtkTextBuffer * tbuf;
	GtkTextIter iter;
	GtkWidget * hbox;
	size_t i;

	if(window != NULL)
	{
		gtk_widget_show(window);
		return;
	}
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
	gtk_container_set_border_width(GTK_CONTAINER(window), 4);
	gtk_window_set_title(GTK_WINDOW(window), "Credits");
	gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(about));
	g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(
				_about_on_closex), NULL);
	vbox = gtk_vbox_new(FALSE, 0);
	textview = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), FALSE);
	tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
	gtk_text_buffer_set_text(tbuf, "", 0);
	for(i = 0; _authors[i] != NULL; i++)
	{
		gtk_text_buffer_get_end_iter(tbuf, &iter);
		gtk_text_buffer_insert(tbuf, &iter, _authors[i], strlen(
					_authors[i]));
	}
	widget = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(widget),
			GTK_SHADOW_ETCHED_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(widget),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(widget), textview);
	notebook = gtk_notebook_new();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), widget,
			gtk_label_new("Written by"));
	gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 4);
	hbox = gtk_hbox_new(FALSE, 0);
	widget = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	g_signal_connect(G_OBJECT(widget), "clicked", G_CALLBACK(
				_about_on_close), window);
	gtk_box_pack_end(GTK_BOX(hbox), widget, FALSE, TRUE, 4);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 4);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show_all(window);
}

static void _about_on_license(GtkWidget * widget, gpointer data)
{
	static GtkWidget * window = NULL;
	GtkWidget * about = data;
	GtkWidget * vbox;
	GtkWidget * textview;
	GtkTextBuffer * tbuf;
	GtkWidget * hbox;

	if(window != NULL)
	{
		gtk_widget_show(window);
		return;
	}
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
	gtk_container_set_border_width(GTK_CONTAINER(window), 4);
	gtk_window_set_title(GTK_WINDOW(window), "License");
	gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(about));
	g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(
				_about_on_closex), NULL);
	vbox = gtk_vbox_new(FALSE, 0);
	textview = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), FALSE);
	tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
	gtk_text_buffer_set_text(tbuf, _license, strlen(_license));
	widget = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(widget),
			GTK_SHADOW_ETCHED_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(widget),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(widget), textview);
	gtk_box_pack_start(GTK_BOX(vbox), widget, TRUE, TRUE, 4);
	hbox = gtk_hbox_new(FALSE, 0);
	widget = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	g_signal_connect(G_OBJECT(widget), "clicked",
			G_CALLBACK(_about_on_close), window);
	gtk_box_pack_end(GTK_BOX(hbox), widget, FALSE, TRUE, 4);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 4);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show_all(window);
}
#endif /* !GTK_CHECK_VERSION(2, 6, 0) */

static void _on_new(GtkWidget * widget, gpointer data)
{
	Editor * editor = data;

	editor_open(editor, NULL);
}

static void _on_open(GtkWidget * widget, gpointer data)
{
	Editor * editor = data;

	editor_open_dialog(editor);
}


void editor_delete(Editor * editor)
{
	free(editor);
}


/* useful */
gboolean editor_close(Editor * editor)
{
	GtkWidget * dialog;
	int ret;

	if(gtk_text_buffer_get_modified(gtk_text_view_get_buffer(GTK_TEXT_VIEW(
						editor->view))) == FALSE)
	{
		gtk_main_quit();
		return FALSE;
	}
	dialog = gtk_message_dialog_new(GTK_WINDOW(editor->window),
			GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO, "%s",
			"Changes are not saved."
			" Are you sure you want to close?");
	ret = gtk_dialog_run(dialog);
	gtk_widget_destroy(dialog);
	if(ret == GTK_RESPONSE_NO)
		return TRUE;
	gtk_main_quit();
	return FALSE;
}


void editor_open(Editor * editor, char const * filename)
{
	FILE * fp;
	GtkTextBuffer * tbuf;
	GtkTextIter iter;
	char buf[BUFSIZ];
	size_t len;

	/* FIXME close previously opened file */
	if(filename == NULL)
		return;
	if((fp = fopen(filename, "r")) == NULL)
	{
		_editor_error(editor, filename, 0);
		return;
	}
	tbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->view));
	gtk_text_buffer_set_text(tbuf, "", 0);
	while((len = fread(buf, sizeof(char), sizeof(buf), fp)) > 0)
	{
		gtk_text_buffer_get_end_iter(tbuf, &iter);
		gtk_text_buffer_insert(tbuf, &iter, buf, len);
	}
	fclose(fp);
}


void editor_open_dialog(Editor * editor)
{
	GtkWidget * dialog;
	int ret;
	char * filename = NULL;

	dialog = gtk_file_chooser_dialog_new("Open file...",
			GTK_WINDOW(editor->window),
			GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, 1,
			GTK_STOCK_OPEN, 0, NULL);
	if((ret = gtk_dialog_run(GTK_DIALOG(dialog))) == GTK_RESPONSE_ACCEPT)
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(
					dialog));
	gtk_widget_destroy(dialog);
	if(filename != NULL)
		editor_open(editor, filename);
}
