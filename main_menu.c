#include "browser.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Declare global variables
extern GtkWidget *main_window;
extern GtkWidget *notebook;

static void select_video_directory_callback(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Select Video Directory",
        GTK_WINDOW(data),
        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Select", GTK_RESPONSE_ACCEPT,
        NULL
    );

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *directory = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        printf("Selected directory: %s\n", directory);

        FILE *config_file = fopen("browser.cfg", "w");
        if (config_file) {
            fprintf(config_file, "video_directory=%s\n", directory);
            fclose(config_file);
        } else {
            printf("Failed to save configuration!\n");
        }

        g_free(directory);
    }

    gtk_widget_destroy(dialog);
}

static void add_bookmark_callback(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Add Bookmark",
        GTK_WINDOW(data),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Add", GTK_RESPONSE_ACCEPT,
        NULL
    );

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *name_entry = gtk_entry_new();
    GtkWidget *url_entry = gtk_entry_new();

    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Name");
    gtk_entry_set_placeholder_text(GTK_ENTRY(url_entry), "URL");

    gtk_box_pack_start(GTK_BOX(content_area), name_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(content_area), url_entry, TRUE, TRUE, 0);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char *name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        const char *url = gtk_entry_get_text(GTK_ENTRY(url_entry));
        add_bookmark(name, url);

        // Update the bookmark toolbar
        GtkWidget *toolbar = GTK_WIDGET(g_object_get_data(G_OBJECT(main_window), "bookmark_toolbar"));
        if (toolbar) {
            update_bookmark_toolbar(toolbar, GTK_NOTEBOOK(notebook));
        }
    }

    gtk_widget_destroy(dialog);
}

GtkWidget *create_main_menu(GtkWidget *window) {
    GtkWidget *menu_bar = gtk_menu_bar_new();

    GtkWidget *config_menu_item = gtk_menu_item_new_with_label("Config");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), config_menu_item);

    GtkWidget *config_submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(config_menu_item), config_submenu);

    GtkWidget *select_video_directory_item = gtk_menu_item_new_with_label("Select Video Directory");
    g_signal_connect(select_video_directory_item, "activate", G_CALLBACK(select_video_directory_callback), window);
    gtk_menu_shell_append(GTK_MENU_SHELL(config_submenu), select_video_directory_item);

    GtkWidget *bookmark_menu_item = gtk_menu_item_new_with_label("Bookmarks");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), bookmark_menu_item);

    GtkWidget *bookmark_submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(bookmark_menu_item), bookmark_submenu);

    GtkWidget *add_bookmark_item = gtk_menu_item_new_with_label("Add Bookmark");
    g_signal_connect(add_bookmark_item, "activate", G_CALLBACK(add_bookmark_callback), window);
    gtk_menu_shell_append(GTK_MENU_SHELL(bookmark_submenu), add_bookmark_item);

    return menu_bar;
}
