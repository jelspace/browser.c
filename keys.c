#include "browser.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// Function to handle key press events
gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    GtkNotebook *notebook = GTK_NOTEBOOK(data);
    GtkWindow *window = GTK_WINDOW(gtk_widget_get_toplevel(widget));

    // Check for Ctrl key combinations
    if (event->state & GDK_CONTROL_MASK) {
        switch (event->keyval) {
            // Ctrl+W: Close the current tab
            case GDK_KEY_w:
            case GDK_KEY_W: {
                int current_page = gtk_notebook_get_current_page(notebook);
                if (current_page != -1) {
                    gtk_notebook_remove_page(notebook, current_page);
                }
                return TRUE; // Event handled
            }

            // Ctrl+T: Open a new tab
            case GDK_KEY_t:
            case GDK_KEY_T: {
                add_tab_callback(NULL, notebook);
                return TRUE; // Event handled
            }

            // Ctrl+Q: Quit the application
            case GDK_KEY_q:
            case GDK_KEY_Q: {
                gtk_main_quit();
                return TRUE; // Event handled
            }

            // Ctrl+R: Reload the current tab
            case GDK_KEY_r:
            case GDK_KEY_R: {
                GtkWidget *current_page = gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook));
                WebKitWebView *web_view = WEBKIT_WEB_VIEW(g_object_get_data(G_OBJECT(current_page), "web_view"));
                webkit_web_view_reload(web_view);
                return TRUE; // Event handled
            }

            // Ctrl+L: Focus the URL entry
            case GDK_KEY_l:
            case GDK_KEY_L: {
                GtkWidget *main_frame = gtk_notebook_get_nth_page(notebook, 0); // Assuming the main frame is the first page
                GtkWidget *url_entry = GTK_WIDGET(g_object_get_data(G_OBJECT(main_frame), "url_entry"));
                if (url_entry) {
                    gtk_widget_grab_focus(url_entry);
                }
                return TRUE; // Event handled
            }

            // Ctrl+F: Toggle fullscreen mode
            case GDK_KEY_f:
            case GDK_KEY_F: {
                if (gtk_window_is_maximized(window)) {
                    gtk_window_unmaximize(window);
                } else {
                    gtk_window_maximize(window);
                }
                return TRUE; // Event handled
            }

            // Ctrl+Plus: Zoom in
            case GDK_KEY_plus:
            case GDK_KEY_equal: { // Equal key is often the same as plus without shift
                GtkWidget *current_page = gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook));
                WebKitWebView *web_view = WEBKIT_WEB_VIEW(g_object_get_data(G_OBJECT(current_page), "web_view"));
                webkit_web_view_set_zoom_level(web_view, webkit_web_view_get_zoom_level(web_view) + 0.1);
                return TRUE; // Event handled
            }

            // Ctrl+Minus: Zoom out
            case GDK_KEY_minus:
            case GDK_KEY_underscore: { // Minus key is often the same as underscore without shift
                GtkWidget *current_page = gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook));
                WebKitWebView *web_view = WEBKIT_WEB_VIEW(g_object_get_data(G_OBJECT(current_page), "web_view"));
                webkit_web_view_set_zoom_level(web_view, webkit_web_view_get_zoom_level(web_view) - 0.1);
                return TRUE; // Event handled
            }

            // Ctrl+0: Reset zoom level
            case GDK_KEY_0:
            case GDK_KEY_parenright: { // 0 key is often the same as parenright without shift
                GtkWidget *current_page = gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook));
                WebKitWebView *web_view = WEBKIT_WEB_VIEW(g_object_get_data(G_OBJECT(current_page), "web_view"));
                webkit_web_view_set_zoom_level(web_view, 1.0); // Reset to default zoom level
                return TRUE; // Event handled
            }
        }
    }

    // Handle F11 for fullscreen
    if (event->keyval == GDK_KEY_F11) {
        if (gtk_window_is_maximized(window)) {
            gtk_window_unmaximize(window);
        } else {
            gtk_window_maximize(window);
        }
        return TRUE; // Event handled
    }

    return FALSE; // Event not handled
}

// Function to connect key press events to the main window
void connect_key_events(GtkWidget *window, GtkNotebook *notebook) {
    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), notebook);
}
