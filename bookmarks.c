#include "browser.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Declare global variables
extern GtkWidget *main_window;
extern GtkWidget *notebook;

// Structure to hold bookmark data
typedef struct {
    char *name;
    char *url;
} Bookmark;

// Array to store bookmarks
static Bookmark bookmarks[1000]; // Increased from 100 to 1000
static int bookmark_count = 0;

// Function to add a bookmark
void add_bookmark(const char *name, const char *url) {
    if (bookmark_count < 1000) {
        bookmarks[bookmark_count].name = g_strdup(name);
        bookmarks[bookmark_count].url = g_strdup(url);
        bookmark_count++;

        // Update the bookmark toolbar
        GtkWidget *toolbar = GTK_WIDGET(g_object_get_data(G_OBJECT(main_window), "bookmark_toolbar"));
        if (toolbar) {
            update_bookmark_toolbar(toolbar, GTK_NOTEBOOK(notebook));
        }
    } else {
        printf("Bookmark limit reached!\n");
    }
}

// Function to remove a bookmark by name
void remove_bookmark(const char *name) {
    for (int i = 0; i < bookmark_count; i++) {
        if (strcmp(bookmarks[i].name, name) == 0) {
            g_free(bookmarks[i].name);
            g_free(bookmarks[i].url);
            for (int j = i; j < bookmark_count - 1; j++) {
                bookmarks[j] = bookmarks[j + 1];
            }
            bookmark_count--;

            // Update the bookmark toolbar
            GtkWidget *toolbar = GTK_WIDGET(g_object_get_data(G_OBJECT(main_window), "bookmark_toolbar"));
            if (toolbar) {
                update_bookmark_toolbar(toolbar, GTK_NOTEBOOK(notebook));
            }
            break;
        }
    }
}

// Function to update the bookmark toolbar
void update_bookmark_toolbar(GtkWidget *toolbar, GtkNotebook *notebook) {
    // Clear the existing toolbar
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(toolbar));
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Rebuild the toolbar with the current bookmarks
    for (int i = 0; i < bookmark_count; i++) {
        GtkWidget *button = GTK_WIDGET(gtk_tool_button_new(NULL, bookmarks[i].name)); // Cast to GtkWidget*
        g_object_set_data(G_OBJECT(button), "notebook", notebook);
        g_signal_connect(button, "clicked", G_CALLBACK(bookmark_button_callback), bookmarks[i].url);
        gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(button), -1);
    }
}

// Function to save bookmarks to a file
void save_bookmarks() {
    FILE *file = fopen("bookmarks.txt", "w");
    if (file) {
        for (int i = 0; i < bookmark_count; i++) {
            fprintf(file, "%s|%s\n", bookmarks[i].name, bookmarks[i].url);
        }
        fclose(file);
    } else {
        printf("Failed to save bookmarks!\n");
    }
}

// Function to load bookmarks from a file
void load_bookmarks() {
    FILE *file = fopen("bookmarks.txt", "r");
    if (file) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            char *name = strtok(line, "|");
            char *url = strtok(NULL, "\n");
            if (name && url) {
                add_bookmark(name, url);
            }
        }
        fclose(file);
    } else {
        printf("No bookmarks file found.\n");
    }
}

// Callback function for bookmark button clicks
void bookmark_button_callback(GtkWidget *widget, gpointer data) {
    const char *url = (const char *)data;

    // Get the current web view from the notebook
    GtkNotebook *notebook = GTK_NOTEBOOK(g_object_get_data(G_OBJECT(widget), "notebook"));
    GtkWidget *current_page = gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook));
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(g_object_get_data(G_OBJECT(current_page), "web_view"));

    webkit_web_view_load_uri(web_view, url);
}

// Function to create the bookmark toolbar
GtkWidget *create_bookmark_toolbar(GtkNotebook *notebook) {
    GtkWidget *toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);

    // Store the toolbar in the main window's data
    g_object_set_data(G_OBJECT(main_window), "bookmark_toolbar", toolbar);

    for (int i = 0; i < bookmark_count; i++) {
        GtkWidget *button = GTK_WIDGET(gtk_tool_button_new(NULL, bookmarks[i].name)); // Cast to GtkWidget*
        g_object_set_data(G_OBJECT(button), "notebook", notebook);
        g_signal_connect(button, "clicked", G_CALLBACK(bookmark_button_callback), bookmarks[i].url);
        gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(button), -1);
    }

    return toolbar;
}
