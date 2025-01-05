#include "browser.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <webkit2/webkit2.h>
#include <gst/gst.h>

// Declare global variables
GtkWidget *main_window; // Global variable to store the main window
GtkWidget *notebook;    // Global variable to store the notebook

void add_tab_callback(GtkWidget *widget, gpointer data) {
    GtkNotebook *notebook = GTK_NOTEBOOK(data);

    // Create a new WebKitWebView
    GtkWidget *web_view = webkit_web_view_new();

    GtkWidget *label = gtk_label_new("New Tab");

    gtk_notebook_append_page(notebook, web_view, label);
    g_object_set_data(G_OBJECT(web_view), "web_view", web_view);
    gtk_widget_show_all(web_view);

    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(web_view), "http://localhost:8000/");
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    gst_init(&argc, &argv); // Initialize GStreamer

    // Set the global WebKit cache model to disable disk caching
    WebKitWebContext *context = webkit_web_context_get_default();
    webkit_web_context_set_cache_model(context, WEBKIT_CACHE_MODEL_DOCUMENT_VIEWER);

    start_server();

    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL); // Store the main window globally
    gtk_window_set_title(GTK_WINDOW(main_window), "Local Video Browser");

    // Get the default display and primary monitor
    GdkDisplay *display = gdk_display_get_default();
    if (display) {
        GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
        if (monitor) {
            GdkRectangle geometry;
            gdk_monitor_get_geometry(monitor, &geometry);
            int width = geometry.width;
            int height = geometry.height;
            gtk_window_set_default_size(GTK_WINDOW(main_window), width, height);
        }
    }

    // Start the browser in fullscreen mode
    gtk_window_fullscreen(GTK_WINDOW(main_window));

    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(main_window), vbox);

    GtkWidget *menu_bar = create_main_menu(main_window);
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

    // Load bookmarks from file
    load_bookmarks();

    // Create the notebook
    notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

    // Create and display the bookmarks toolbar
    GtkWidget *bookmark_toolbar = create_bookmark_toolbar(GTK_NOTEBOOK(notebook));
    gtk_box_pack_start(GTK_BOX(vbox), bookmark_toolbar, FALSE, FALSE, 0);

    GtkWidget *main_frame = create_main_frame(GTK_NOTEBOOK(notebook));
    gtk_box_pack_start(GTK_BOX(vbox), main_frame, FALSE, FALSE, 0);

    GtkWidget *add_tab_button = gtk_button_new_with_label("+");
    g_signal_connect(add_tab_button, "clicked", G_CALLBACK(add_tab_callback), notebook);
    gtk_box_pack_start(GTK_BOX(vbox), add_tab_button, FALSE, FALSE, 0);

    add_tab_callback(NULL, GTK_NOTEBOOK(notebook));
    connect_key_events(main_window, GTK_NOTEBOOK(notebook));

    gtk_widget_show_all(main_window);
    gtk_main();

    // Save bookmarks to file before exiting
    save_bookmarks();

    return 0;
}
